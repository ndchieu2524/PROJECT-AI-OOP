#pragma once

#include <string>

#include "llm_client.h"

namespace llm {
    class OllamaClient: public LLMClient {
        private:
            std::string baseURL_;
            Options options_;

        public:
            explicit OllamaClient(const std::string baseURL, Options options);

            Response chat(const std::vector<Message> &messages, const Options &options = {}) const override;
    };
}