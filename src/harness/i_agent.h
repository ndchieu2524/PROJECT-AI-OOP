#pragma once
#include <string>
#include <functional>
#include "trajectory.h"

using StepHook = std::function<void(const AgentStep&)>;

class IAgent {
public:
    virtual ~IAgent() = default;
    virtual void set_step_hook(StepHook hook) = 0;
    virtual std::string run(const std::string& prompt, Trajectory& trajectory) = 0;
};