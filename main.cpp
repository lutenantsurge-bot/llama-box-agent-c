#include <iostream>
#include <fstream>
#include <filesystem>
#include <map>

// Tool implementations
std::string file_search(const std::string& query, const std::string& path) {
    std::string results;
    try {
        for (auto& entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.path().filename().string().find(query) != std::string::npos) {
                results += entry.path().string() + "\n";
            }
        }
    } catch (const std::exception& e) {
        results = std::string("Error: ") + e.what();
    }
    return results.empty() ? "No results found." : results;
}

std::string read_file(const std::string& filepath) {
    std::ifstream f(filepath);
    if (!f) return "Error: cannot open file " + filepath;
    return std::string(std::istreambuf_iterator<char>(f),
                       std::istreambuf_iterator<char>());
}

std::string write_file(const std::string& filepath, const std::string& content) {
    std::ofstream f(filepath);
    if (!f) return "Error: cannot write to " + filepath;
    f << content;
    return "Written: " + filepath;
}

// Tool definitions (JSON Schema)
const std::string TOOLS_JSON = R"([
  {
    "type": "function",
    "function": {
      "name": "file_search",
      "description": "Search for files by name in a directory tree",
      "parameters": {
        "type": "object",
        "properties": {
          "query": {
            "type": "string",
            "description": "Filename or pattern to search for"
          },
          "path": {
            "type": "string",
            "description": "Root directory to search from",
            "default": "."
          }
        },
        "required": ["query"]
      }
    }
  },
  {
    "type": "function",
    "function": {
      "name": "read_file",
      "description": "Read the full contents of a file",
      "parameters": {
        "type": "object",
        "properties": {
          "filepath": {
            "type": "string",
            "description": "Absolute or relative path to the file"
          }
        },
        "required": ["filepath"]
      }
    }
  },
  {
    "type": "function",
    "function": {
      "name": "write_file",
      "description": "Write content to a file, creating it if it does not exist",
      "parameters": {
        "type": "object",
        "properties": {
          "filepath": {
            "type": "string",
            "description": "Path to write to"
          },
          "content": {
            "type": "string",
            "description": "Content to write"
          }
        },
        "required": ["filepath", "content"]
      }
    }
  }
])";

// Tool dispatcher
std::string dispatch_tool(const std::string& name,
                           const std::map<std::string, std::string>& args) {
    if (name == "file_search") {
        std::string path = args.count("path") ? args.at("path") : ".";
        return file_search(args.at("query"), path);
    }
    if (name == "read_file")  return read_file(args.at("filepath"));
    if (name == "write_file") return write_file(args.at("filepath"), args.at("content"));
    return "Unknown tool: " + name;
}

int main(int argc, char* argv[]) {
    std::cout << "llama-box + agent.cpp combined project\n";
    std::cout << "========================================\n\n";
    std::cout << "This combines:\n";
    std::cout << "  - llama-box: Inference server with multimodal support\n";
    std::cout << "  - agent.cpp: C++ agent loop with tool calling\n\n";
    std::cout << "Architecture:\n";
    std::cout << "  GGUF model (+ mmproj) -> llama-box server :8080\n";
    std::cout << "  -> agent.cpp client (tool defs, loop, memory)\n";
    std::cout << "  -> your main.cpp (glue, tool implementations)\n\n";
    
    std::cout << "Tools available:\n";
    std::cout << "  - file_search: Search files by name\n";
    std::cout << "  - read_file: Read file contents\n";
    std::cout << "  - write_file: Write to file\n\n";
    
    std::cout << "Agent ready. Type your request (or 'exit'):\n> ";
    
    std::string user_input;
    while (std::getline(std::cin, user_input)) {
        if (user_input == "exit") break;
        std::cout << "\nProcessing: " << user_input << "\n";
        std::cout << "> ";
    }
    
    return 0;
}
