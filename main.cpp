#include <iostream>
#include "include/command_line/CommandLine.h"
#include "include/conversion/EncodingConverter.h"
#include "include/xml/HelpText.h"
#include "include/config_parser/Parser.h"
#include "include/file_reader/TextFileReader.h"

void print_duration(const auto& start) {
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    auto duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Время выполнения: " << duration_microseconds.count() << " microseconds" << std::endl;
    std::cout << "Время выполнения: " << duration_milliseconds.count() << " milliseconds" << std::endl;
}

std::string find_file_by_name(const std::string& directory, const std::string& model_name) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().filename().string().find(model_name) != std::string::npos) {
            return entry.path().string();
        }
    }
    throw std::runtime_error("File not found: " + model_name);
}

void test(){
    try {
        auto config_parser = ConfigParser::create("./path/path.json");

        std::string tbl_directory = config_parser->getFilePath("tbl");
        std::vector<std::string> tbl_files = config_parser->getFileList("tbl", "scale");

        std::vector<tbl> tbl_data;
        for (const std::string& tbl_file : tbl_files) {
            tbl temp_tbl;
            TxtFileReader(tbl_file).read_tbl(temp_tbl);
            tbl_data.push_back(temp_tbl);
        }
//        for(auto  i : tbl_data){
//            std::cout << i.position << std::endl;
//            std::cout << i.model << std::endl;
//            for(auto &j : i.measurements){
//                for( auto k : j){
//                    std::cout << k << " ";
//                }
//                std::cout << std::endl;
//            }
//            std::cout << std::endl;
//        }
        std::string cfm_file = config_parser->getFilePath("cfm");
        std::string cfm_file_path = find_file_by_name(cfm_file, tbl_data[0].model);
        cfm cfm_data;
        TxtFileReader(cfm_file_path).read_cfm(cfm_data);

        std::string zgt_file = config_parser->getFilePath("zgt");
        std::string zgt_file_path = find_file_by_name(zgt_file, cfm_data.blank);
        std::cout << zgt_file_path << std::endl;
        zgt zgt_data;
        TxtFileReader(zgt_file_path).read_zgt(zgt_data);
//
//        std::string nbr_file = config_parser->getFilePath("nbr");
//        nbr nbr_data;
//        TxtFileReader(nbr_file).read_nbr(nbr_data);
//
//        std::string point_file = config_parser->getFilePath("point");
//        point point_data;
//        JsonFileReader(point_file).read_point(point_data);

        // Здесь передаем данные в класс PDFRendering и выполняем обработку данных
        // PDFRendering pdf_rendering(tbl_data, cfm_data, zgt_data, nbr_data, point_data);
        // pdf_rendering.process();

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
}

int main(int argc, char *argv[]) {
    auto start = std::chrono::high_resolution_clock::now();

    CommandLine cmd(argc, argv);
    auto command_type = cmd.get_command_type();
    auto args = cmd.get_args();

    if (command_type == CommandLine::CommandType::CONVERT) {
        if(args.size() == 1){
            EncodingConverter converter(args[0]);
            converter.convert();
        } else if (args.size() == 2) {
            EncodingConverter converter(args[0], args[1]);
            converter.convert();
        } else {
            std::cerr << "Неверное количество аргументов." << std::endl;
            return 1;
        }
    } else if (command_type == CommandLine::CommandType::GENERATE) {
        // вызов функции генерации
        ConfigParser::create(args[0]); // Читает файлы конфигурации
    } else if (command_type == CommandLine::CommandType::PRINT) {
        // вызов функции печати
    } else {
        test();
//        std::string help_text = read_help_text();
//        if (!help_text.empty()) {
//            std::cout << help_text << std::endl;
//        }
//        std::cerr << "Неизвестная команда или недостаточно аргументов." << std::endl;
//        return 1;
    }
    print_duration(start);
    return 0;
}
