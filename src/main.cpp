#include <string>
#include <format>
#include <print>

#include <curl/curl.h>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "client/llm_client.h"
#include "client/ollama_client.h"

using namespace std;
using namespace agent::llm;

string LINK = "http://mhpgc-34-125-101-225.free.pinggy.net";

int main() {
    println("okay dude");
    // OllamaClient client(LINK, Options({
    //   .modelName = "gemma4:e4b"
    // })); 

    // vector<Message> messages;
    
    // messages.push_back(Message({
    //   .role = "user",
    //   .content = "what is 1 + 1"
    // }));

    // Response res = client.chat(messages);

    // println("Content: {}", res.content);
    // println("Finish: {}", res.finish);
    // println("Reason: {}", res.finishReason);
    // println("Used tokens: ", res.usedTokens);

    return 0;
}