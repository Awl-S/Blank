//
// Created by orys on 18.04.23.
//

#ifndef BLANK_ZGT_H
#define BLANK_ZGT_H

#include <vector>
#include <array>

struct zgt{
    double diameter;
    double center_hole_diameter;
    std::vector<double> mounting_holes;
    std::vector<double> marker_position;
    std::array<double, 2> font_serial_number{}; // добавить
    std::array<double, 3> stop_position{}; // добавить
};

#endif //BLANK_ZGT_H
