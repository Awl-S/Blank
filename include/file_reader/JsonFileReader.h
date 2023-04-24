#ifndef BLANK_JSONFILEREADER_H
#define BLANK_JSONFILEREADER_H

#include "FileReader.h"

class JsonFileReader : public FileReader {
public:
    explicit JsonFileReader(const std::string& file_path) : FileReader(file_path) {}
    // Реализация методов для чтения JSON файлов

    void read_cfm(cfm& data) {

    }
    void read_nbr(nbr& data) {

    }
    void read_zgt(zgt& data) {

    }
    void read_point(point& data) {

    }
    void read_tbl(tbl& data) {

    }
};

#endif //BLANK_JSONFILEREADER_H
