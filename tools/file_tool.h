#pragma once

#include <filesystem>

#include "tool.h"

class FileTool : public Tool {
public:
    explicit FileTool(std::filesystem::path sandboxRoot);

    std::string name() const override;
    std::string description() const override;
    nlohmann::json parametersSchema() const override;
    nlohmann::json execute(const nlohmann::json& args) override;

private:
    std::filesystem::path sandboxRoot_;
    std::filesystem::path resolveSafePath(const std::string& relativePath) const;
};
