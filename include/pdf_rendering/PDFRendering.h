#ifndef BLANK_PDFRENDERING_H
#define BLANK_PDFRENDERING_H

#include <utility>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <sstream>
#include <iomanip>
#include "../struct/cfm.h"
#include "../struct/zgt.h"
#include "../struct/tbl.h"
#include "../struct/point_data.h"
#include "../struct/nbr.h"
#include "hpdf.h"

class PDFRendering {

    HPDF_Font loadFont(const std::string& fontPath);
    void sortTblData();
    void populatePointTbl();
    bool checkForNewPage();

    void createA3Page();

    void setFontSize(HPDF_Page page, double size);

    static void drawCircle(HPDF_Page page, double x, double y, double radius);

    static void draw_line(HPDF_Page page, HPDF_REAL x1, HPDF_REAL y1, HPDF_REAL x2, HPDF_REAL y2);

    void generate_Marks(size_t blank);

    void drawDigitIndices(HPDF_Page page, size_t blank);

    void drawMainLines(HPDF_Page page, size_t blank);

    void drawAdditionalTicks(HPDF_Page page, size_t blank);

public:
    void generatePDF(const std::string& filename);

    PDFRendering(cfm  cfm_data, zgt  zgt_data, const std::vector<tbl>& tbl_data,
                 nbr  nbr_data, point  point_data);

    ~PDFRendering();

private:
    std::vector<double> marks;
    bool new_page = false;
    std::vector<size_t> point_tbl;
    HPDF_Doc pdf_;
    HPDF_Font font_;

    cfm cfm_data_;
    zgt zgt_data_;
    nbr nbr_data_;
    point point_data_;
    std::vector<tbl> tbl_data_;
};

#endif //BLANK_PDFRENDERING_H