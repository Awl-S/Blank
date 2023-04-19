#ifndef BLANK_FILEREADER_H
#define BLANK_FILEREADER_H
#include <memory>
#include <iostream>
#include <fstream>
#include <sstream>
#include <utility>
#include <vector>
#include <array>
#include <string>
#include <regex>
#include <nlohmann/json.hpp>
#include <tinyxml2.h>

#include "../struct/cfm.h"
#include "../struct/tbl.h"
#include "../struct/zgt.h"

class FileReader {
public:
    // Конструктор для инициализации файла
    explicit FileReader(std::string  file_path) : file_path_(std::move(file_path)) {}

    // Остальные методы FileReader ...

protected:
    std::string file_path_;
    static std::string readLineString(std::ifstream &file)
    {
        std::string line;
        if (std::getline(file, line))
        {
            // Remove any leading/trailing whitespace
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            line.erase(line.find_last_not_of(" \t\r\n") + 1);
        }
        return line;
    }
};

#endif //BLANK_FILEREADER_H
