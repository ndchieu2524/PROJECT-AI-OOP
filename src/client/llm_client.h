#pragma once

#include <string>
#include <vector>

namespace llm {
    struct Message {
        std::string role;
        std::string content;
    };

    struct Options {
        std::string modelName;
        
        double temperature = 0.7;
        double maxTokens = 512;
    };

    struct Response {
        std::string content;
        std::string finishReason;
        bool finish = false;

        int usedTokens = 0;
        
        // debug
        std::string rawJson = "";
    };

    class LLMClient {
        public:
            virtual ~LLMClient() = default;

            virtual Response chat(const std::vector<Message> &messages, const Options &options = {}) const = 0;
    };
}