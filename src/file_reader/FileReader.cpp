#include "../../include/file_reader/FileReader.h"
#include "../../include/file_reader/JsonFileReader.h"
#include "../../include/file_reader/XmlFileReader.h"
#include "../../include/file_reader/TextFileReader.h"

static std::shared_ptr<FileReader> create(const std::string& file_path) {
    std::filesystem::path p(file_path);
    std::string extension = p.extension().string();
    if (extension == ".json") {
        return std::make_shared<JsonFileReader>(file_path);
    } else if (extension == ".help") {
        return std::make_shared<XmlFileReader>(file_path);
    } else if (extension == ".txt" || extension == ".cfm" || extension == ".zgt" || extension == ".nbr") {
        return std::make_shared<TxtFileReader>(file_path);
    } else {
        throw std::runtime_error("Unsupported file extension: " + extension);
    }
}