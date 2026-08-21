#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <nlohmann/json.hpp>
#include "../harness/i_agent.h"
#include "../harness/trajectory.h"
#include "../client/llm_client.h"
#include "../tools/tool_registry.h"
#include "skill_loader.h"
#include "loop_detector.h"

class AgentLoop : public IAgent {
private:
    std::shared_ptr<agent::llm::LLMClient> llm_client_;
    std::shared_ptr<agent::tools::ToolRegistry> tool_registry_;
    std::shared_ptr<agent::skills::SkillLoader> skill_loader_;
    agent::engine::LoopDetector loop_detector_;
    
    int max_steps_{10};
    StepHook step_hook_{nullptr};

    std::string build_system_prompt(const std::string& task_instruction);

    struct ParsedAction {
        bool is_tool_call{false};
        std::string tool_name;
        nlohmann::json tool_args;
        std::string final_answer;
        std::string thought;
    };
    ParsedAction parse_llm_response(const std::string& response);

protected:
    virtual std::string observe(const std::string& observation);
    virtual agent::llm::Response think(const std::vector<agent::llm::Message>& history);
    virtual nlohmann::json act(const std::string& tool_name, const nlohmann::json& tool_args);

public:
    AgentLoop(std::shared_ptr<agent::llm::LLMClient> llm,
              std::shared_ptr<agent::tools::ToolRegistry> tools,
              std::shared_ptr<agent::skills::SkillLoader> skills,
              int max_steps = 10);

    void set_step_hook(StepHook hook) override { step_hook_ = hook; }
    
    std::string run(const std::string& prompt, Trajectory& trajectory) override;
};