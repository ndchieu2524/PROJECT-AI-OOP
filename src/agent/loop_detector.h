#pragma once

#include <string>
#include <vector>
#include <algorithm>

namespace agent::engine {
    enum class LoopSeverity {
        NORMAL,
        WARNING,
        CRITICAL
    };

    class LoopDetector {
        private:
            int warningLimit;
            int criticalLimit;
            std::vector<std::string> actionHistory;

            int countConsecutiveRepeats() const;
            int countPingPongLoops() const;

        public:
            LoopDetector(int warningLim = 3, int criticalLim = 5);
            
            void addActionToHistory(const std::string& action);
            LoopSeverity evaluateLoopStatus() const;
            void clearHistory();
            
            void setWarningLimit(int limit);
            void setCriticalLimit(int limit);
            int getWarningLimit() const;
            int getCriticalLimit() const;
    };
}