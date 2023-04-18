#ifndef BLANK_XMLCONFIGPARSER_H
#define BLANK_XMLCONFIGPARSER_H

#include "ConfigParser.h"

class XMLConfigParser : public ConfigParser {
public:
    void display() override;

    explicit XMLConfigParser(const std::string& config_file_path);

    std::string getFilePath(const std::string& key) override;

    std::vector<std::string> getFileList(const std::string& key, const std::string& search) override;

private:
    tinyxml2::XMLDocument xml_doc;
    const tinyxml2::XMLElement* config_element;
};

#endif //BLANK_XMLCONFIGPARSER_H
