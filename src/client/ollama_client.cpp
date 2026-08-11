#include <format>
#include <print>
#include <string>
#include <vector>
#include <exception>
#include <cmath>
#include <curl/curl.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "ollama_client.h"
#include "utils/http_client.h"

using namespace std;
using namespace agent::llm;
using namespace agent::net;

OllamaClient::OllamaClient(const std::string baseURL, Options options): baseURL_(baseURL), options_(options) {
    if (options_.modelName.size() == 0) {
        throw invalid_argument("You must specify a model name in your options.");
    }
}

Response OllamaClient::chat(const vector<Message> &messages, const Options &options) const {
    Options choosedOptions = options.modelName.size() != 0 ? options : options_;
    json compiledMessages = json::array();

    for (Message m : messages) {
        json jsonMessage = {
            {"role", m.role},
            {"content", m.content}
        };
        compiledMessages.push_back(jsonMessage);
    }

    json payload = { 
        {"model", choosedOptions.modelName},
        {"messages", compiledMessages},
        {"stream", false}
    };

    if (choosedOptions.temperature) {
        payload["options.temperature"] = choosedOptions.temperature;
    }

    if (choosedOptions.maxTokens) {
        payload["options.num_predict"] = choosedOptions.maxTokens;
    }

    const string postBody = payload.dump();
    string postURL = format("{}/api/chat", baseURL_);

    HttpResponse httpRes = post(postURL, postBody);

    if (!httpRes.isSuccess()) {
        return Response({
            .finishReason = httpRes.errorMessage,
            .finish = false
        });
    }

    json parsedRes = json::parse(httpRes.result);
    
    return Response({
        .content = parsedRes["message"],
        .finishReason = parsedRes["done_reason"],
        .finish = parsedRes["done"],
        .usedTokens = parsedRes["eval_count"],
        .rawJson = httpRes.result
    });
}
