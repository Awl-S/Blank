#ifndef BLANK_JSONCONFIGPARSER_H
#define BLANK_JSONCONFIGPARSER_H

#include <fstream>
#include "ConfigParser.h"

class JSONConfigParser : public ConfigParser {
public:
    void display() override;

    explicit JSONConfigParser(const std::string& config_file_path);

    std::string getFilePath(const std::string& key) override;

    std::vector<std::string> getFileList(const std::string& key, const std::string& search) override;

private:
    nlohmann::json config_json;
};
#endif //BLANK_JSONCONFIGPARSER_H
