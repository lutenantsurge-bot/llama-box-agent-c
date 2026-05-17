#include "model.h"
#include "agent.h"

#include <iostream>
#include <sstream>
#include <regex>

Model::Model(const ModelConfig& config) : config_(config) {
    std::cout << "[Model] Initializing with config:\n";
    std::cout << "  base_url: " << config_.base_url << "\n";
    std::cout << "  model: " << config_.model << "\n";
    std::cout << "  n_ctx: " << config_.n_ctx << "\n";
    std::cout << "  temperature: " << config_.temperature << "\n";
}

std::string Model::chat(const std::vector<Message>& messages) {
    std::ostringstream prompt;
    prompt << "<|begin_of_text|>";
    
    for (const auto& msg : messages) {
        if (msg.role == "user") {
            prompt << "<|start_header_id|>user<|end_header_id|>\n\n" << msg.content << "<|eot_id|>";
        } else if (msg.role == "assistant") {
            prompt << "<|start_header_id|>assistant<|end_header_id|>\n\n" << msg.content << "<|eot_id|>";
        } else if (msg.role == "tool") {
            prompt << "<|start_header_id|>tool<|end_header_id|>\n\n" << msg.content << "<|eot_id|>";
        }
    }
    
    prompt << "<|start_header_id|>assistant<|end_header_id|>\n\n";
    
    // Simulate response (in real implementation, would call llama-box API)
    std::string simulated_response = "This is a simulated response. In the real implementation, this would call the llama-box server at " + config_.base_url;
    
    std::cout << "[Model] Chat completed. Response length: " << simulated_response.size() << " chars\n";
    
    return simulated_response;
}

std::vector<ToolCall> Model::parse_tool_calls(const std::string& response) {
    std::vector<ToolCall> tool_calls;
    
    // Simple parsing of tool call format
    // In real implementation, would parse llama-box tool call JSON format
    std::regex tool_regex(R"("name":\s*"([^"]+)".*"arguments":\s*"([^"]+)"\)");
    std::sregex_iterator iter(response.begin(), response.end(), tool_regex);
    std::sregex_iterator end;
    
    while (iter != end) {
        ToolCall tc;
        tc.name = (*iter)[1].str();
        tc.arguments_json = (*iter)[2].str();
        
        // Simple argument parsing
        std::string args = (*iter)[2].str();
        std::regex arg_regex(R"("([^"]+)":\s*"([^"]+)"\)");
        std::sregex_iterator arg_iter(args.begin(), args.end(), arg_regex);
        std::sregex_iterator arg_end;
        
        while (arg_iter != arg_end) {
            tc.arguments[(*arg_iter)[1].str()] = (*arg_iter)[2].str();
            ++arg_iter;
        }
        
        tool_calls.push_back(tc);
        ++iter;
    }
    
    return tool_calls;
}
