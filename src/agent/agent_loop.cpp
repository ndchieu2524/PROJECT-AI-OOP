#include "agent_loop.h"
#include <chrono>
#include <iostream>
#include <regex>
using namespace std;
using json = nlohmann::json;

AgentLoop::AgentLoop(shared_ptr<agent::llm::LLMClient> llm,
                     shared_ptr<agent::tools::ToolRegistry> tools,
                     shared_ptr<agent::skills::SkillLoader> skills,
                     int max_steps)
    : llm_client_(move(llm)),
      tool_registry_(move(tools)),
      skill_loader_(move(skills)),
      loop_detector_(3, 5),
      max_steps_(max_steps) {}

string AgentLoop::build_system_prompt(const string& task_instruction) {
    string prompt = "You are an AI Agent solving tasks via tool use.\n"
                         "Respond strictly in the following format:\n"
                         "Thought: <your step-by-step reasoning>\n"
                         "Action: tool_call | final_answer\n"
                         "Action Input: {\"tool\": \"<tool_name>\", \"args\": <arguments_json>} OR <final_answer_text>\n\n"
                         "Available Tools:\n";

    if (tool_registry_) {
        for (const auto& tool_name : tool_registry_->listToolNames()) {
            auto tool = tool_registry_->getTool(tool_name);
            if (tool) {
                prompt += "- " + tool->name() + ": " + tool->description() + "\n";
            }
        }
    }

    if (skill_loader_) {
        string skill = skill_loader_->selectSkillForTask(task_instruction);
        if (!skill.empty()) {
            prompt += "\nSkill Guideline:\n" + skill + "\n";
        }
    }
    return prompt;
}

AgentLoop::ParsedAction AgentLoop::parse_llm_response(const string& response) {
    ParsedAction res;
    res.tool_args = json::object();
    
    regex thought_rgx(R"(Thought:\s*([\s\S]*?)(?=\nAction:|$))");
    smatch match;
    if (regex_search(response, match, thought_rgx)) {
        res.thought = match[1].str();
    } else {
        res.thought = response;
    }

    regex action_rgx(R"(Action:\s*(tool_call|final_answer)\s*\nAction Input:\s*([\s\S]*))");
    if (regex_search(response, match, action_rgx)) {
        string type = match[1].str();
        string input_str = match[2].str();

        if (type == "tool_call") {
            try {
                auto j = json::parse(input_str);
                res.is_tool_call = true;
                res.tool_name = j.value("tool", "");
                if (j.contains("args")) {
                    res.tool_args = j["args"];
                }
            } catch (...) {
                res.is_tool_call = false;
                res.final_answer = input_str;
            }
        } else {
            res.is_tool_call = false;
            res.final_answer = input_str;
        }
    } else {
        res.is_tool_call = false;
        res.final_answer = response;
    }
    return res;
}

string AgentLoop::observe(const string& observation) {
    return "Observation: " + observation;
}

agent::llm::Response AgentLoop::think(const vector<agent::llm::Message>& history) {
    if (!llm_client_) {
        return agent::llm::Response{.content = "Error: LLM Client is null"};
    }
    return llm_client_->chat(history);
}

nlohmann::json AgentLoop::act(const string& tool_name, const nlohmann::json& tool_args) {
    if (!tool_registry_) {
        return json{{"success", false}, {"error", "Tool registry not available"}};
    }
    return tool_registry_->execute(tool_name, tool_args);
}

string AgentLoop::run(const string& prompt, Trajectory& trajectory) {
    loop_detector_.clearHistory();
    vector<agent::llm::Message> history;

    string sys_prompt = build_system_prompt(prompt);
    history.push_back({"system", sys_prompt, {}});
    history.push_back({"user", prompt, {}});

    int current_step = 0;
    string final_output = "";

    while (current_step < max_steps_) {
        auto start_time = chrono::steady_clock::now();

        // THINK
        agent::llm::Response llm_resp = think(history);
        string raw_llm_response = llm_resp.content;

        // PARSE ACTION
        ParsedAction action = parse_llm_response(raw_llm_response);

        // LOOP DETECTION
        string action_signature = action.is_tool_call 
            ? (action.tool_name + ":" + action.tool_args.dump()) 
            : "final_answer";
            
        loop_detector_.addActionToHistory(action_signature);
        agent::engine::LoopSeverity status = loop_detector_.evaluateLoopStatus();
        
        if (status == agent::engine::LoopSeverity::CRITICAL) {
            cerr << "[AgentLoop] Critical loop detected at step " << current_step << ". Terminating.\n";
            final_output = "Terminated due to critical loop detection.";
            break;
        }

        auto end_time = chrono::steady_clock::now();
        long long latency_ms = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

        // ACT & RECORD STEP
        if (!action.is_tool_call) {
            final_output = action.final_answer;
            AgentStep step{
                .step_number = current_step,
                .thought = action.thought,
                .action_name = "final_answer",
                .action_args = "",
                .observation = action.final_answer,
                .latency_ms = latency_ms
            };
            trajectory.addStep(step);
            if (step_hook_) step_hook_(step);
            break;
        }

        json tool_exec_res = act(action.tool_name, action.tool_args);
        string tool_res_str = tool_exec_res.dump();

        AgentStep step{
            .step_number = current_step,
            .thought = action.thought,
            .action_name = action.tool_name,
            .action_args = action.tool_args.dump(),
            .observation = tool_res_str,
            .latency_ms = latency_ms
        };
        trajectory.addStep(step);
        if (step_hook_) step_hook_(step);

        history.push_back({"assistant", raw_llm_response, {}});
        history.push_back({"user", observe(tool_res_str), {}});

        current_step++;
    }

    if (current_step >= max_steps_ && final_output.empty()) {
        final_output = "Exceeded maximum allowed steps.";
    }

    return final_output;
}