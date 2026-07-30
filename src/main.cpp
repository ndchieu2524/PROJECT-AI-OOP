#include <string>
#include <format>
#include <print>

#include <curl/curl.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "client/llm_client.h"
#include "client/ollama_client.h"

using namespace std;

string LINK = "http://mhpgc-34-125-101-225.free.pinggy.net";

int main() {
    llm::OllamaClient client(LINK, llm::Options({
      .modelName = "gemma4:e4b"
    })); 

    vector<llm::Message> messages;
    
    messages.push_back(llm::Message({
      .role = "user",
      .content = "what is 1 + 1"
    }));

    llm::Response res = client.chat(messages);

    println("Content: {}", res.content);
    println("Finish: {}", res.finish);
    println("Reason: {}", res.finishReason);
    println("Used tokens: ", res.usedTokens);

    return 0;
}