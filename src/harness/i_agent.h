#pragma once
#include <string>
#include "trajectory.h"

class IAgent {
public:
    virtual ~IAgent() = default;
    virtual std::string run(const std::string& prompt, Trajectory& trajectory) = 0;
};