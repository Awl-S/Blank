#include "../../include/config_parser/JSONConfigParser.h"

std::vector<std::string> JSONConfigParser::getFileList(const std::string &key, const std::string &search) {
    std::vector<std::string> files;
    std::string directory = getFilePath(key);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().filename().string().find(search) != std::string::npos) {
            files.push_back(entry.path().string());
        }
    }
    return files;
}

std::string JSONConfigParser::getFilePath(const std::string &key) {
    if (!config_json.contains(key)) {
        throw std::runtime_error("Configuration file does not contain key: " + key);
    }
    return config_json[key];
}

JSONConfigParser::JSONConfigParser(const std::string &config_file_path) {
    std::ifstream config_file(config_file_path);
    if (!config_file.is_open()) {
        throw std::runtime_error("Cannot open configuration file: " + config_file_path);
    }
    config_file >> config_json;
}

void JSONConfigParser::display() {
    for (const auto& [key, value] : config_json.items()) {
        std::cout << key << ": " << value << std::endl;
    }
}