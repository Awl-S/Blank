//
// Created by orys on 18.04.23.
//

#ifndef BLANK_TBL_H
#define BLANK_TBL_H

#include <string>
#include <vector>

struct tbl {
    int position;
    std::string model;
    std::vector<std::vector<double>> measurements;
};

#endif //BLANK_TBL_H
