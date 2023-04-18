#include "../../include/config_parser/TXTConfigParser.h"

TXTConfigParser::TXTConfigParser(const std::string &config_file_path) {
    std::ifstream config_file(config_file_path);
    if (!config_file.is_open()) {
        throw std::runtime_error("Cannot open configuration file: " + config_file_path);
    }

    std::string line;
    while (std::getline(config_file, line)) {
        std::istringstream line_stream(line);
        std::string key, value;
        if (line_stream >> key >> value) {
            config_data[key] = value;
        }
    }
}

void TXTConfigParser::display() {
    for (const auto& [key, value] : config_data) {
        std::cout << key << ": " << value << std::endl;
    }
}

std::string TXTConfigParser::getFilePath(const std::string &key) {
    auto it = config_data.find(key);
    if (it == config_data.end()) {
        throw std::runtime_error("Configuration file does not contain key: " + key);
    }
    return it->second;
}

std::vector<std::string> TXTConfigParser::getFileList(const std::string &key, const std::string &search) {
    std::vector<std::string> files;
    std::string directory = getFilePath(key);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().filename().string().find(search) != std::string::npos) {
            files.push_back(entry.path().string());
        }
    }

    return files;
}
