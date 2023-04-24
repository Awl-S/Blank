#ifndef BLANK_PARSER_H
#define BLANK_PARSER_H

#include "../../include/config_parser/ConfigParser.h"
#include "../../include/config_parser/JSONConfigParser.h"
#include "../../include/config_parser/XMLConfigParser.h"
#include "../../include/config_parser/TXTConfigParser.h"

std::unique_ptr<ConfigParser> ConfigParser::create(const std::string& config_file_path) {
    if (config_file_path.ends_with(".json")) {
        return std::make_unique<JSONConfigParser>(config_file_path);
    } else if (config_file_path.ends_with(".help")) {
        return std::make_unique<XMLConfigParser>(config_file_path);
    } else if (config_file_path.ends_with(".txt")) {
        return std::make_unique<TXTConfigParser>(config_file_path);
    } else {
        throw std::runtime_error("Unsupported file type: " + config_file_path);
    }
}
#endif //BLANK_PARSER_H
