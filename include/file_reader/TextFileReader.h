#ifndef BLANK_TEXTFILEREADER_H
#define BLANK_TEXTFILEREADER_H

#include "FileReader.h"

class TxtFileReader : public FileReader {
public:
    explicit TxtFileReader(const std::string& file_path) : FileReader(file_path) {}

    void read_nbr(nbr& data) {
        std::ifstream file(file_path_);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + file_path_);
        }

        std::string line;
        size_t line_number = 1;
        while (std::getline(file, line)) {
            data.icon.insert(std::make_pair(line_number, line));
            line_number++;
        }
    }

    void read_point(point& data)  {
        std::ifstream file(file_path_);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + file_path_);
        }

        double x, y;
        while (file >> x >> y) {
            data.coordinates.emplace_back(x, y);
        }
    }

    void read_tbl(tbl& data) {
        std::ifstream file(file_path_);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + file_path_);
        }
        std::string file_name = file_path_.substr(file_path_.find_last_of("/") + 1);
        std::regex pattern("\\d+");
        if (std::smatch matches; std::regex_search(file_name, matches, pattern)) {
            data.position = std::stoi(matches[0]);
        }

        std::string line;
        readLineString(file);
        data.model = readLineString(file);

        data.measurements.reserve(20);
        while (std::getline(file, line)) {
            double temp;
            double col2;
            double col3;
            if (std::istringstream(line) >> temp >> col2 >> col3) {
                data.measurements.push_back({col2, col3});
            }
        }
    }

    void read_cfm(cfm& data) {
        std::ifstream file(file_path_);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + file_path_);
        }

        std::string line;
        size_t count = 1;
        while (std::getline(file, line)) {
            if (count == 1) {
                data.header = trim(line);
            } else if (count == 3) {
                data.blank = trim(line);
            } else if (count == 6) {
                std::istringstream iss(line);
                double temp;
                while (iss >> temp) {
                    data.marks.push_back(temp);
                }
            } else if (count == 8) {
                std::istringstream(line) >> data.format[0] >> data.format[1];
            } else if (count == 10) {
                std::istringstream(line) >> data.diameter;
            } else if (count == 12) {
                std::istringstream(line) >> data.small_tick[0] >> data.small_tick[1];
            } else if (count == 14) {
                std::istringstream(line) >> data.big_tick[0] >> data.big_tick[1];
            } else if (count == 16) {
                std::istringstream(line) >> data.digital_tick[0] >> data.digital_tick[1] >> data.digital_tick[2] >> data.digital_tick[3];
            } else if (count == 18) {
                std::istringstream(line) >> data.digit_height >> data.label_height;
            } else if (count == 20) {
                data.tick_mask = trim(line);
            }
            count++;
        }
    }

    void read_zgt(zgt& data)  {

        std::ifstream file(file_path_);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + file_path_);
        }

        size_t count = 1;
        std::string line;
        while (std::getline(file, line)) {
            if (count == 2) {
                std::istringstream(line) >> data.diameter;
            } else if (count == 4) {
                std::istringstream(line) >> data.center_hole_diameter;
            } else if (count == 6) {
                std::istringstream iss(line);
                double temp;
                while (iss >> temp) {
                    data.mounting_holes.push_back(temp);
                }
            } else if (count == 8) {
                std::istringstream iss(line);
                double temp;
                while (iss >> temp) {
                    data.marker_position.push_back(temp);
                }
            } else if (count == 10) {
                std::istringstream(line) >> data.font_serial_number[0] >> data.font_serial_number[1];
            } else if (count == 12) {
                std::istringstream(line) >> data.stop_position[0] >> data.stop_position[1] >> data.stop_position[2];
            }
            count++;
        }
    }

// Реализация функции trim


    // Реализация методов для чтения TXT файлов
    // Пример: read_cfm(), read_zgt(), read_nbr(), read_point(), read_tbl()
};

#endif //BLANK_TEXTFILEREADER_H
