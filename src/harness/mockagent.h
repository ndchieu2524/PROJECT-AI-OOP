#pragma once
#include <string>
#include "i_agent.h"

class MockAgent : public IAgent {
public:
    std::string run(const std::string& prompt, Trajectory& trajectory) override;
};