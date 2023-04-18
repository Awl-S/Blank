#include "../../include/conversion/file_converter.h"
#include <filesystem>
#include <string>
#include <random>
#include <iostream>
#include <fstream>
#include <cstring>

bool FileConverter::convert_to_utf8(const std::string& input_file, const std::string& output_file) {
    std::filesystem::path start_input(input_file);
    return false;
}

bool FileConverter::convert_to_utf8(const std::string& input_file) {
    std::filesystem::path start_input(input_file);
    auto temp_directory = create_temp_directory();
    copyDirectories(input_file, temp_directory);

    for (const auto& entry : std::filesystem::recursive_directory_iterator(input_file)) {
        if (std::filesystem::is_regular_file(entry.path())) {
            auto ext = entry.path().extension();
            auto relative_path = entry.path().lexically_relative(input_file);

            if (ext == ".cfm" || ext == ".tbl" || ext == ".згт") {
                convertFile(input_file / relative_path,
                            temp_directory / relative_path);
            } else {
                std::filesystem::copy(input_file / relative_path, temp_directory / relative_path);
            }
        }
    }

    for (const auto& entry : std::filesystem::directory_iterator(temp_directory)) {
        if (std::filesystem::is_regular_file(entry.path())) {

            std::filesystem::path dest = input_file / entry.path().filename();

            if (std::filesystem::exists(dest) && is_same_file(entry, dest)) {
                std::filesystem::remove(dest);
            }
            std::filesystem::rename(entry, dest);
        }
    }
    std::filesystem::remove_all(temp_directory);

    return true;
}

FileConverter::FileConverter() {
    iconv_descriptor = iconv_open("UTF-8", "CP1251");
    generator = std::default_random_engine(time(nullptr));
}

FileConverter::~FileConverter() {
    iconv_close(iconv_descriptor);
}

std::string FileConverter::generate_random_string() {
    static const std::string chars = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::uniform_int_distribution<size_t> distribution(0, chars.size() - 1);

    std::string random_string;
    random_string.reserve(10);

    for (size_t i = 0; i < 10; ++i) {
        random_string.push_back(chars[distribution(generator)]);
    }

    return "ManotomBlank_"+random_string;
}

std::filesystem::path FileConverter::create_temp_directory() {
    auto temp_directory_path = std::filesystem::temp_directory_path() / generate_random_string();

    while (std::filesystem::exists(temp_directory_path)) {
        temp_directory_path = std::filesystem::temp_directory_path() / generate_random_string();
    }

    std::filesystem::create_directories(temp_directory_path);
    return temp_directory_path;
}

void FileConverter::copyDirectories(std::filesystem::path source, std::filesystem::path target) {
    std::filesystem::create_directories(target);
    for (const auto& entry : std::filesystem::directory_iterator(source)) {
        if (std::filesystem::is_directory(entry)) {
            copyDirectories(entry, target / entry.path().filename());
        }
    }
}

bool FileConverter::convertFile(const std::string &input_file, const std::string &output_file) {
    if (iconv_descriptor == (iconv_t)-1) {
        std::cerr << "iconv_open error: " << strerror(errno) << std::endl;
        return false;
    }

    std::ifstream ifs(input_file);
    if (!ifs.is_open()) {
        std::cerr << "Failed to open input file: " << strerror(errno) << std::endl;
        return false;
    }

    std::ofstream ofs(output_file);
    if (!ofs.is_open()) {
        std::cerr << "Failed to open output file: " << strerror(errno) << std::endl;
        return false;
    }

    if(is_cp1251(input_file)){
        std::string line;
        while (std::getline(ifs, line)) {
            char *inbuf = const_cast<char *>(line.c_str());
            size_t inbytesleft = line.length();
            size_t outbytesleft = inbytesleft * 4;
            char *outbuf = new char[outbytesleft];

            char *inptr = inbuf;
            char *outptr = outbuf;

            if (iconv(iconv_descriptor, &inptr, &inbytesleft, &outptr, &outbytesleft) == (size_t)-1) {
                std::cerr << "iconv error: " << strerror(errno) << std::endl;
                std::cerr << input_file << std::endl;
                delete[] outbuf;
                return false;
            }
            *outptr = '\0';

            ofs << outbuf << std::endl;

            delete[] outbuf;
        }
    }
    return true;
}

bool FileConverter::is_same_file(const std::filesystem::path &path1, const std::filesystem::path &path2) {
    std::ifstream file1(path1, std::ios::binary);
    std::ifstream file2(path2, std::ios::binary);

    if (!file1.is_open() || !file2.is_open()) {
        return false;
    }

    std::istreambuf_iterator<char> it1(file1), it2(file2), end;

    return std::equal(it1, end, it2, end);
}

bool FileConverter::is_cp1251(const std::string &file_path) {
    std::ifstream file(file_path, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Ошибка открытия файла" << std::endl;
        return false;
    }

    std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
    file.close();

    char* inbuf = buffer.data();
    size_t inbytesleft = buffer.size();
    std::vector<char> outbuf(inbytesleft * 2);
    char* outbufptr = outbuf.data();
    size_t outbytesleft = outbuf.size();

    size_t res = iconv(iconv_descriptor, &inbuf, &inbytesleft, &outbufptr, &outbytesleft);

    return res != (size_t)-1;
}
