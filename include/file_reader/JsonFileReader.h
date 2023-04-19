#ifndef BLANK_JSONFILEREADER_H
#define BLANK_JSONFILEREADER_H

#include "FileReader.h"

class JsonFileReader : public FileReader {
public:
    explicit JsonFileReader(const std::string& file_path) : FileReader(file_path) {}

    // Реализация методов для чтения JSON файлов
    // Пример: read_cfm(), read_zgt(), read_nbr(), read_point(), read_tbl()
};

#endif //BLANK_JSONFILEREADER_H
