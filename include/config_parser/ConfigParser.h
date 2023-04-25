#ifndef BLANK_CONFIGPARSER_H
#define BLANK_CONFIGPARSER_H
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <nlohmann/json.hpp>
#include "tinyxml2.h"

class ConfigParser {
public:
    virtual ~ConfigParser() = default;
    virtual std::string getFilePath(const std::string& key) = 0;
    virtual std::vector<std::string> getFileList(const std::string& key, const std::string& search) = 0;
    [[maybe_unused]] virtual void display() = 0;
    static std::unique_ptr<ConfigParser> create(const std::string& config_file_path);
};

#endif //BLANK_CONFIGPARSER_H