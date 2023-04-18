#ifndef BLANK_COMMANDLINE_H
#define BLANK_COMMANDLINE_H
#include <string>
#include <vector>

class CommandLine {
public:
    CommandLine(int argc, char *argv[]);

    enum class CommandType {
        CONVERT,
        GENERATE,
        PRINT,
        UNKNOWN
    };

    CommandType get_command_type() const;
    std::vector<std::string> get_args() const;

private:
    CommandType command_type;
    std::vector<std::string> args;
};

#endif //BLANK_COMMANDLINE_H
