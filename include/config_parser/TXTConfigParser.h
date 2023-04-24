#ifndef BLANK_TXTCONFIGPARSER_H
#define BLANK_TXTCONFIGPARSER_H

#include <fstream>
#include "ConfigParser.h"

class TXTConfigParser : public ConfigParser {
public:
    void display() override;

    explicit TXTConfigParser(const std::string& config_file_path);

    std::string getFilePath(const std::string& key) override;

    std::vector<std::string> getFileList(const std::string& key, const std::string& search) override;

private:
    std::map<std::string, std::string> config_data;
};

#endif //BLANK_TXTCONFIGPARSER_H