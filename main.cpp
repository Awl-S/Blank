#include <iostream>
#include "include/command_line/CommandLine.h"
#include "include/conversion/EncodingConverter.h"
#include "include/help/HelpText.h"
#include "src/config_parser/Parser.h"
#include "include/file_reader/TextFileReader.h"
#include "include/struct/point_data.h"
#include "include/pdf_rendering/PDFRendering.h"

void print_duration(const auto& start) {
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    auto duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Время выполнения: " << duration_microseconds.count() << " microseconds" << std::endl;
    std::cout << "Время выполнения: " << duration_milliseconds.count() << " milliseconds" << std::endl;
}

std::string find_file_by_name(const std::string& directory, const std::string& model_name, const std::string& extension) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            const auto& file_name = entry.path().filename().string();
            const auto& file_extension = entry.path().extension().string();

            if (file_name == model_name + file_extension && file_extension == extension) {
                return entry.path().string();
            }
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

        std::string cfm_file = config_parser->getFilePath("cfm");
        std::string cfm_file_path = find_file_by_name(cfm_file, tbl_data[0].model, ".cfm");
        cfm cfm_data;
        TxtFileReader(cfm_file_path).read_cfm(cfm_data);
        std::cout << cfm_file_path << std::endl;

        std::string zgt_file = config_parser->getFilePath("zgt");
        std::string zgt_file_path = find_file_by_name(zgt_file, cfm_data.blank, ".згт");
        zgt zgt_data;
        TxtFileReader(zgt_file_path).read_zgt(zgt_data);
        std::cout << zgt_file_path << std::endl;

//        std::string nbr_file = config_parser->getFilePath("nbr");
//        std::string nbr_file_path = find_file_by_name(nbr_file, "", ".нбр");
        nbr nbr_data;
//        TxtFileReader(nbr_file).read_nbr(nbr_data);

        std::string point_file = config_parser->getFilePath("point");
        std::string point_file_path = find_file_by_name(point_file, tbl_data[0].model, ".point");
        point point_data;
        TxtFileReader(point_file_path).read_point(point_data);
        std::cout << point_file_path << std::endl;

        // Здесь передаем данные в класс PDFRendering и выполняем обработку данных
        PDFRendering pdf_rendering(cfm_data, zgt_data, tbl_data, nbr_data, point_data);
        pdf_rendering.generatePDF("test.pdf");

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
        try{

        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
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
