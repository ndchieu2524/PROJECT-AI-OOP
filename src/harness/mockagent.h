#pragma once
#include <iostream>
#include <thread>
#include <chrono>
#include "i_agent.h"
using namespace std;

class MockAgent : public IAgent {
public:
    string run(const string& prompt, Trajectory& trajectory) override {
        this_thread::sleep_for(chrono::milliseconds(50));

        AgentStep step1;
        step1.step_number = 1;
        step1.thought = "Đang phân tích yêu cầu bài test: " + prompt;
        
        if (prompt.find("15 + 25") != string::npos) {
            step1.action_name = "calculator";
            step1.action_args = "15 + 25";
            step1.observation = "40";
            step1.latency_ms = 85;
            trajectory.addStep(step1);
            return "Kết quả phép tính là 40";
        } 
        else if (prompt.find("thủ đô") != string::npos) {
            step1.action_name = "search_wiki";
            step1.action_args = "Thủ đô Việt Nam";
            step1.observation = "Hà Nội";
            step1.latency_ms = 110;
            trajectory.addStep(step1);
            return "Thủ đô của Việt Nam là Hà Nội";
        }
        step1.action_name = "default_tool";
        step1.action_args = prompt;
        step1.observation = "Thành công";
        step1.latency_ms = 60;
        trajectory.addStep(step1);
        return "Đã hoàn thành yêu cầu: " + prompt;
    }
};