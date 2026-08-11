#include <string>
#include <format>
#include <print>
#include <curl/curl.h>

#include "client/llm_client.h"
#include "http_client.h"

using namespace std;
using namespace agent::llm;

namespace {
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
        ((string*) userp)->append((char*) contents, size * nmemb);
        return size * nmemb;
    }
}

namespace agent::net {
    bool HttpResponse::isSuccess() const {
        return statusCode >= 200 && statusCode < 300;
    }
}

namespace agent::net {
    HttpResponse post(const string &url, const string &body) {
        CURL *curl = curl_easy_init();

        if (!curl) {
            return HttpResponse ({
                .statusCode = 0,
                .errorMessage = "Cannot init CURL"
            });
        }

        int statusCode = 0;

        string buffer;

        CURLcode res;

        /*
        TODO: Nên thêm function để lựa chọn headers (cái này chỉ để tạm thời)
         */
        struct curl_slist *headers = nullptr;
        headers = curl_slist_append(headers, "Content-Type: application/json");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        res = curl_easy_perform(curl);

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &statusCode);

        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            return HttpResponse({
                .statusCode = 0,
                .errorMessage = curl_easy_strerror(res)
            });
        } 

        HttpResponse httpRes({.statusCode = statusCode});

        if (!httpRes.isSuccess()) {
            httpRes.errorMessage = buffer;
        } else {
            httpRes.result = buffer;
        }

        return httpRes;
    }
}