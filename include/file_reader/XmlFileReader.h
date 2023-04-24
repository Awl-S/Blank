#ifndef BLANK_XMLFILEREADER_H
#define BLANK_XMLFILEREADER_H

#include "FileReader.h"

class XmlFileReader : public FileReader {
public:
    explicit XmlFileReader(const std::string& file_path) : FileReader(file_path) {}
    // Реализация методов для чтения XML файлов
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

#endif //BLANK_XMLFILEREADER_H
