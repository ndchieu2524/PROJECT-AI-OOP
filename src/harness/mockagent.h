#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include <memory>
#include "i_agent.h"
#include "agent/skill_loader.h"
#include "agent/loop_detector.h"

using namespace std;
using namespace agent::skills;
using namespace agent::engine;

class MockAgent : public IAgent {
private:
    shared_ptr<SkillLoader> skillLoader;

public:
    MockAgent(shared_ptr<SkillLoader> loader = nullptr) : skillLoader(loader) {}

    string run(const string& prompt, Trajectory& trajectory) override {
        string systemPrompt = "Bạn là một AI Agent trợ lý.";
        if (skillLoader) {
            string selectedSkill = skillLoader->selectSkillForTask(prompt);
            if (!selectedSkill.empty()) {
                string skillContent = skillLoader->getSkillContent(selectedSkill);
                skillLoader->injectSkillIntoPrompt(systemPrompt, skillContent);
            }
        }

        LoopDetector loopDetector(3, 5);

        for (int step = 1; step <= 3; ++step) {
            this_thread::sleep_for(chrono::milliseconds(30));

            string currentAction = "calculator(15+25)";
            
            loopDetector.addActionToHistory(currentAction);
            LoopSeverity severity = loopDetector.evaluateLoopStatus();

            if (severity == LoopSeverity::CRITICAL) {
                cerr << "[CẢNH BÁO AGENT] Dừng thực thi do phát hiện vòng lặp vô tận!" << endl;
                break;
            }

            AgentStep agentStep;
            agentStep.step_number = step;
            agentStep.thought = "Đang xử lý yêu cầu với Prompt: " + systemPrompt;
            agentStep.action_name = "calculator";
            agentStep.action_args = "15+25";
            agentStep.observation = "40";
            agentStep.latency_ms = 30;

            trajectory.addStep(agentStep);
        }

        if (prompt.find("15 + 25") != string::npos || prompt.find("15 + 25") != string::npos) {
            return "40.0";
        } else if (prompt.find("thủ đô") != string::npos) {
            return "Thủ đô của Việt Nam là Hà Nội";
        }

        return "2026-08-12";
    }
};