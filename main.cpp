#include <iostream>
#include "include/command_line/CommandLine.h"
#include "include/conversion/file_converter.h"
#include "include/conversion/EncodingConverter.h"

int main(int argc, char *argv[]) {
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
    } else if (command_type == CommandLine::CommandType::PRINT) {
        // вызов функции печати
    } else {
        std::cerr << "Неизвестная команда или недостаточно аргументов." << std::endl;
        return 1;
    }
    return 0;
}
