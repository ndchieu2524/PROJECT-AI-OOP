#include "tool_registry.h"

void ToolRegistry::registerTool(std::shared_ptr<Tool> tool) {
    if (!tool) return;
    tools_[tool->name()] = std::move(tool);
}

bool ToolRegistry::hasTool(const std::string& toolName) const {
    return tools_.find(toolName) != tools_.end();
}

std::shared_ptr<Tool> ToolRegistry::getTool(const std::string& toolName) const {
    auto it = tools_.find(toolName);
    if (it == tools_.end()) return nullptr;
    return it->second;
}

std::vector<std::string> ToolRegistry::listToolNames() const {
    std::vector<std::string> names;
    names.reserve(tools_.size());
    for (const auto& [name, tool] : tools_) {
        names.push_back(name);
    }
    return names;
}

nlohmann::json ToolRegistry::listToolSchemas() const {
    nlohmann::json schemas = nlohmann::json::array();
    for (const auto& [name, tool] : tools_) {
        schemas.push_back({
            {"name", tool->name()},
            {"description", tool->description()},
            {"parameters", tool->parametersSchema()}
        });
    }
    return schemas;
}

nlohmann::json ToolRegistry::execute(const std::string& toolName,
                                      const nlohmann::json& args) const {
    auto it = tools_.find(toolName);
    if (it == tools_.end()) {
        return {
            {"success", false},
            {"error", "Tool '" + toolName + "' không tồn tại trong registry"}
        };
    }

    try {
        return it->second->execute(args);
    } catch (const std::exception& e) {
        return {
            {"success", false},
            {"error", std::string("Exception khi thực thi tool: ") + e.what()}
        };
    }
}
