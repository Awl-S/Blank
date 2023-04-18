#include "../../include/config_parser/XMLConfigParser.h"

void XMLConfigParser::display() {
    if (config_element) {
        for (const tinyxml2::XMLElement* element = config_element->FirstChildElement();
             element != nullptr; element = element->NextSiblingElement()) {
            std::cout << element->Name() << ": " << element->GetText() << std::endl;
        }
    } else {
        throw std::runtime_error("Configuration element not found in the XML file");
    }
}

XMLConfigParser::XMLConfigParser(const std::string &config_file_path) {
    tinyxml2::XMLError result = xml_doc.LoadFile(config_file_path.c_str());
    if (result != tinyxml2::XML_SUCCESS) {
        throw std::runtime_error("Cannot open configuration file: " + config_file_path);
    }
    config_element = xml_doc.FirstChildElement("configuration");
    if (!config_element) {
        throw std::runtime_error("Configuration element not found in the XML file");
    }
}

std::string XMLConfigParser::getFilePath(const std::string &key) {
    auto element = xml_doc.FirstChildElement(key.c_str());
    if (element == nullptr) {
        throw std::runtime_error("Configuration file does not contain key: " + key);
    }
    return element->GetText();
}

std::vector<std::string> XMLConfigParser::getFileList(const std::string &key, const std::string &search) {
    std::vector<std::string> files;
    std::string directory = getFilePath(key);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().filename().string().find(search) != std::string::npos) {
            files.push_back(entry.path().string());
        }
    }

    return files;
}
