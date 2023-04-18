#include <iostream>
#include "include/command_line/CommandLine.h"
#include "include/conversion/EncodingConverter.h"
#include "include/xml/HelpText.h"
#include "include/config_parser/Parser.h"

void print_duration(const auto& start) {
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    auto duration_milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Время выполнения: " << duration_microseconds.count() << " microseconds" << std::endl;
    std::cout << "Время выполнения: " << duration_milliseconds.count() << " milliseconds" << std::endl;
}

int main(int argc, char *argv[]) {
    auto start = std::chrono::high_resolution_clock::now();

    CommandLine cmd(argc, argv);
    auto command_type = cmd.get_command_type();
    auto args = cmd.get_args();
    auto config_parser = ConfigParser::create("/home/orys/Desktop/setup/path/path.xml");
    config_parser->display();
//    if (command_type == CommandLine::CommandType::CONVERT) {
//        if(args.size() == 1){
//            EncodingConverter converter(args[0]);
//            converter.convert();
//        } else if (args.size() == 2) {
//            EncodingConverter converter(args[0], args[1]);
//            converter.convert();
//        } else {
//            std::cerr << "Неверное количество аргументов." << std::endl;
//            return 1;
//        }
//
//    } else if (command_type == CommandLine::CommandType::GENERATE) {
//        // вызов функции генерации
//        ConfigParser::create(args[0]);
//    } else if (command_type == CommandLine::CommandType::PRINT) {
//        // вызов функции печати
//    } else {
//        std::string help_text = read_help_text();
//        if (!help_text.empty()) {
//            std::cout << help_text << std::endl;
//        }
//        std::cerr << "Неизвестная команда или недостаточно аргументов." << std::endl;
//        return 1;
//    }
//    print_duration(start);
//    return 0;
}
