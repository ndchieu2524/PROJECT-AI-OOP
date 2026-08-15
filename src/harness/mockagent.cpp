#include "mockagent.h"
#include <thread>
#include <chrono>
using namespace std;

string MockAgent::run(const string& prompt, Trajectory& trajectory) {
    this_thread::sleep_for(chrono::milliseconds(90));

    AgentStep step;
    step.step_number = 1;
    step.thought = "Đang phân tích dữ liệu cho yêu cầu: " + prompt;
    step.action_name = "default_tool";
    step.action_args = prompt;
    step.observation = "OK";
    step.latency_ms = 90;
    trajectory.addStep(step);

    if (prompt.find("15 + 25") != string::npos) {
        return "40.0";
    } else if (prompt.find("thủ đô") != string::npos) {
        return "Thủ đô của Việt Nam là Hà Nội";
    }

    return "2026-08-12";
}