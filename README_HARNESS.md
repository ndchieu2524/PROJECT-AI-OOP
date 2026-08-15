# HƯỚNG DẪN TÍCH HỢP AI AGENT VÀO MODULE HARNESS

## Yêu cầu kế thừa Interface `IAgent`

Để Agent có thể chạy được trên bộ Benchmark, class Agent cần:
1. `#include "harness/i_agent.h"`
2. Kế thừa từ class `IAgent`.
3. Ghi từng bước ReAct (Thought, Action, Observation, Latency) vào đối tượng `Trajectory`.

### Code mẫu:

```cpp
#include "harness/i_agent.h"
#include <chrono>

class ReActAgent : public IAgent {
public:
    string run(const string& prompt, Trajectory& trajectory) override {
        for (int step = 1; step <= max_steps; ++step) {
            auto start_time = chrono::high_resolution_clock::now();
            
            auto end_time = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(end_time - start_time).count();

            AgentStep current_step;
            current_step.step_number = step;
            current_step.thought = "Thought từ LLM...";
            current_step.action_name = "tên_tool";
            current_step.action_args = "đối_số_tool";
            current_step.observation = "kết_quả_tool";
            current_step.latency_ms = duration;
            
            trajectory.addStep(current_step);

            if (is_final_answer) {
                return final_answer_string;
            }
        }
        return "Không hoàn thành";
    }
};