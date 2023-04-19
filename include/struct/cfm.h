#ifndef BLANK_CFM_H
#define BLANK_CFM_H

#include <string>
#include <vector>
#include <array>
struct cfm {
    std::string header;
    std::string blank;
    std::vector<double> marks;
    std::array<int, 2> format;
    int diameter;
    std::array<double, 2> small_tick;
    std::array<double, 2> big_tick;
    std::array<double, 4> digital_tick;
    int digit_height;
    int label_height;
    std::string tick_mask;
};
#endif //BLANK_CFM_H