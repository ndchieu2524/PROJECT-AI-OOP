#include "loop_detector.h"
#include <iostream>
#include <print>

using namespace std;
using namespace agent::engine;

LoopDetector::LoopDetector(int warningLim, int criticalLim): warningLimit(warningLim), criticalLimit(criticalLim) {}

void LoopDetector::addActionToHistory(const string& action) {
    actionHistory.push_back(action);
}

void LoopDetector::clearHistory() {
    actionHistory.clear();
}

void LoopDetector::setWarningLimit(int limit) {
    warningLimit = limit;
}

void LoopDetector::setCriticalLimit(int limit) {
    criticalLimit = limit;
}

int LoopDetector::getWarningLimit() const {
    return warningLimit;
}

int LoopDetector::getCriticalLimit() const {
    return criticalLimit;
}

int LoopDetector::countConsecutiveRepeats() const {
    if (actionHistory.empty()) return 0;
    
    int repeatCount = 1;
    const string& lastAction = actionHistory.back();
    
    for (int i = static_cast<int>(actionHistory.size()) - 2; i >= 0; --i) {
        if (actionHistory[i] == lastAction) {
            repeatCount++;
        } else {
            break;
        }
    }
    return repeatCount;
}

int LoopDetector::countPingPongLoops() const {
    if (actionHistory.size() < 4) return 0;
    
    int pingPongCount = 0;
    size_t n = actionHistory.size();
    
    string actionA = actionHistory[n - 1];
    string actionB = actionHistory[n - 2];
    
    if (actionA == actionB) return 0; 
    
    for (int i = static_cast<int>(n) - 1; i >= 1; i -= 2) {
        if (actionHistory[i] == actionA && actionHistory[i - 1] == actionB) {
            pingPongCount++;
        } else {
            break;
        }
    }
    
    return pingPongCount;
}

LoopSeverity LoopDetector::evaluateLoopStatus() const {
    int repeatCount = countConsecutiveRepeats();
    int pingPongCount = countPingPongLoops();
    
    int maxLoop = max(repeatCount, pingPongCount);
    
    if (maxLoop >= criticalLimit) {
        println("[Nghiêm trọng] Phát hiện vòng lặp. Dừng agent");
        return LoopSeverity::CRITICAL;

    } else if (maxLoop >= warningLimit) {
        println("[Cảnh báo] Phát hiện khả năng có vòng lặp");
        return LoopSeverity::WARNING;
    }
    
    return LoopSeverity::NORMAL;
}
