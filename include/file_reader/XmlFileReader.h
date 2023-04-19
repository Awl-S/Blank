#ifndef BLANK_XMLFILEREADER_H
#define BLANK_XMLFILEREADER_H

#include "FileReader.h"

class XmlFileReader : public FileReader {
public:
    XmlFileReader(const std::string& file_path) : FileReader(file_path) {}

    // Реализация методов для чтения XML файлов
    // Пример: read_cfm(), read_zgt(), read_nbr(), read_point(), read_tbl()
};

#endif //BLANK_XMLFILEREADER_H
