#pragma once

#include <string>

namespace agent::net {
    struct HttpResponse {
        int statusCode = 0;
        std::string result;
        std::string errorMessage;

        bool isSuccess() const;
    };

    HttpResponse post(const std::string &url, const std::string &body);
}
