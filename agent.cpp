#include "agent.h"
#include "model.h"
#include "callback.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include <functional>
#include <mutex>

namespace agent_cpp {

Agent::Agent(const ModelConfig& config)
    : config_(config)
    , model_(std::make_unique<Model>(config))
    , tool_dispatcher_(nullptr)
    , memory_file_("")
    , memory_mutex_()
{
    std::cout << "[Agent] Initializing...\n";
    std::cout << "  - Model: " << config_.model << "\n";
    std::cout << "  - Base URL: " << config_.base_url << "\n";
    std::cout << "  - Context: " << config_.n_ctx << "\n";
    std::cout << "  - Temperature: " << config_.temperature << "\n";
    std::cout << "  - Tools count: " << count_brackets(config_.tools_json) << "\n";
}

void Agent::set_tool_dispatcher(const ToolDispatcher& dispatcher) {
    tool_dispatcher_ = dispatcher;
}

void Agent::enable_memory(const std::string& memory_file) {
    memory_file_ = memory_file;
    
    // Create directory if needed
    std::filesystem::path p(memory_file);
    if (p.has_parent_path()) {
        std::filesystem::create_directories(p.parent_path());
    }
    
    std::cout << "[Agent] Memory enabled: " << memory_file << "\n";
}

std::string Agent::run(const std::string& user_message) {
    std::cout << "[Agent] Running with message: " << user_message << "\n";
    
    // Add user message to context
    messages_.push_back({"role": "user", "content": user_message});
    
    // Get model response
    std::string response = model_->chat(messages_);
    
    // Parse tool calls from response
    auto tool_calls = model_->parse_tool_calls(response);
    
    if (!tool_calls.empty()) {
        std::cout << "[Agent] Tool calls detected: " << tool_calls.size() << "\n";
        
        for (const auto& tc : tool_calls) {
            std::string name = tc.name;
            std::map<std::string, std::string> args = tc.arguments;
            
            std::cout << "  - Tool: " << name << "\n";
            std::cout << "    Args: " << tc.arguments_json << "\n";
            
            if (tool_dispatcher_) {
                std::string result = tool_dispatcher_(name, args);
                std::cout << "    Result: " << result << "\n";
                
                // Add tool response to messages
                messages_.push_back({
                    "role": "tool",
                    "name": name,
                    "content": result
                });
            }
        }
        
        // Get follow-up response
        response = model_->chat(messages_);
    }
    
    // Add assistant response to messages
    messages_.push_back({"role": "assistant", "content": response});
    
    return response;
}

size_t Agent::count_brackets(const std::string& json) {
    size_t count = 0;
    for (char c : json) {
        if (c == '{') count++;
    }
    return count;
}

} // namespace agent_cpp
