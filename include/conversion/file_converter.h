#ifndef BLANK_FILE_CONVERTER_H
#define BLANK_FILE_CONVERTER_H

#include <filesystem>
#include <random>
#include "iconv.h"
#include "string"

class FileConverter {
public:
    FileConverter();
    ~FileConverter();
    bool convert_to_utf8(const std::string& input_file, const std::string& output_file);
    bool convert_to_utf8(const std::string& input_file);

private:
    std::string generate_random_string();
    std::filesystem::path create_temp_directory();
    void copyDirectories(std::filesystem::path source, std::filesystem::path target);
    bool convertFile(const std::string& input_file, const std::string& output_file);
    bool is_same_file(const std::filesystem::path& path1, const std::filesystem::path& path2);
    bool is_cp1251(const std::string& file_path);

    iconv_t iconv_descriptor;
    std::default_random_engine generator;
};

#endif //BLANK_FILE_CONVERTER_H
