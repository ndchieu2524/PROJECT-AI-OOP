#pragma once
#include <string>
#include "trajectory.h"
using namespace std;

class IAgent {
public:
    virtual ~IAgent() = default;
    virtual string run(const string& prompt, Trajectory& trajectory) = 0;
};