#include "../include/common_utils.h"
#include "../include/json_utils/json_utils.h"
#include <iostream>
#include <string>
#include <sstream>
#include <map>
#include <regex>
#include <algorithm>
#include <fstream>

JsonManager::JsonManager(DebugLogger* logger) : logger_(logger) {}

std::map<std::string, std::string> JsonManager::readJson(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonStr = buffer.str();
    file.close();

    return parseJson(jsonStr);
}

void JsonManager::writeJson(const std::string& filePath, const std::map<std::string, std::string>& data) {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filePath);
    }

    file << "{\n";
    bool first = true;
    for (const auto& [key, value] : data) {
        if (!first) file << ",\n";
        file << "  \"" << key << "\": \"" << value << "\"";
        first = false;
    }
    file << "\n}\n";
    file.close();
}

std::map<std::string, std::string> JsonManager::parseConfig(const std::string& jsonStr) {
    return parseJson(jsonStr);
}

std::map<std::string, std::string> JsonManager::parseJson(const std::string& jsonStr) {
    // Remove whitespace
    std::string str = jsonStr;
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());

    // Basic JSON parsing
    std::map<std::string, std::string> config;
    std::regex pattern(R"(\"([^\"]+)\"\s*:\s*\"?([^,\"}]+)\"?)");
    std::smatch matches;

    auto begin = str.cbegin();
    auto end = str.cend();

    while (std::regex_search(begin, end, matches, pattern)) {
        if (matches.size() >= 3) {
            std::string key = matches[1].str();
            std::string value = matches[2].str();
            
            // Remove quotes if present
            if (value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            config[key] = value;
        }
        begin = matches[0].second;
    }

    if (logger_ && logger_->isDebugMode()) {
        std::cout << "Parsed JSON config:" << std::endl;
        for (const auto& [key, value] : config) {
            std::cout << key << ": " << value << std::endl;
        }
    }

    return config;
}
