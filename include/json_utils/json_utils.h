#ifndef JSON_H
#define JSON_H

#include <string>
#include <map>

class DebugLogger {
public:
    virtual bool isDebugMode() const = 0;
    virtual ~DebugLogger() = default;
};

class JsonManager {
public:
    JsonManager(DebugLogger* logger);
    
    std::map<std::string, std::string> readJson(const std::string& filePath);
    void writeJson(const std::string& filePath, const std::map<std::string, std::string>& data);
    std::map<std::string, std::string> parseConfig(const std::string& jsonStr);

private:
    std::map<std::string, std::string> parseJson(const std::string& jsonStr);
    DebugLogger* logger_;
};

#endif
