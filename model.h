#pragma once

#include <string>
#include <vector>
#include <map>

struct Message {
    std::string role;
    std::string content;
    std::string name;
};

struct ModelConfig {
    std::string base_url;
    std::string model;
    int n_ctx;
    double temperature;
    std::string tools_json;
};

struct ToolCall {
    std::string name;
    std::string arguments_json;
    std::map<std::string, std::string> arguments;
};

class Model {
public:
    Model(const ModelConfig& config);
    
    std::string chat(const std::vector<Message>& messages);
    std::vector<ToolCall> parse_tool_calls(const std::string& response);
    
private:
    ModelConfig config_;
};
