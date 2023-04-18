#ifndef BLANK_ENCODINGCONVERTER_H
#define BLANK_ENCODINGCONVERTER_H

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <iconv.h>
#include <vector>
#include "tinyxml2.h"


class EncodingConverter {
public:
    EncodingConverter(std::string  inputDir, const std::string& outputDir = "");
    void convert();

private:
    std::string inputDirectory;
    std::string outputDirectory;
    bool useTempDirectory;
    std::vector<std::string> fileExtensions;
    std::string sourceEncoding;
    std::string targetEncoding;
    size_t filesProcessed;
    size_t filesConverted;

    void loadSettingsFromXml();
    void convertDirectory(const std::filesystem::path& dir);
    void convertFile(const std::filesystem::path& filepath);
    std::string iconvConvert(const std::string& input, const std::string& fromCharset, const std::string& toCharset);
    void printStats();
    bool iconvConvert(const std::string& input, std::string& output, const std::string& fromCharset, const std::string& toCharset);

    bool is_utf8(const std::string& str);

};

#endif //BLANK_ENCODINGCONVERTER_H
