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
public:
    PDFRendering(cfm  cfm_data, zgt  zgt_data, const std::vector<tbl>& tbl_data,
                 nbr  nbr_data, point  point_data)
            : cfm_data_(std::move(cfm_data)), zgt_data_(std::move(zgt_data)), tbl_data_(tbl_data),
              nbr_data_(std::move(nbr_data)), point_data_(std::move(point_data)) {
        pdf_ = HPDF_New(nullptr, nullptr);
        if(!pdf_){
            throw std::runtime_error("Не удалось создать PDF документ.");
        }

        auto font_name = HPDF_LoadTTFontFromFile(pdf_, "/home/orys/Desktop/setup/font/GOST_A.ttf", HPDF_TRUE);
        font_ = HPDF_GetFont(pdf_, font_name, "CP1251");

        if(!font_){
            throw std::runtime_error("Не удалось загрузить шрифт.");
        }

        auto compare_position = [](const tbl& a, const tbl& b) {
            return a.position < b.position;
        };

        // Сортировка вектора tbl_data_ с использованием функции сравнения
        std::sort(tbl_data_.begin(), tbl_data_.end(), compare_position);

        for(auto i : tbl_data_){
            point_tbl.push_back(i.position);
        }

        for (size_t i = 0; i < point_data_.coordinates.size(); ++i) {
            for (size_t j = i + 1; j < point_data_.coordinates.size(); ++j) {
                if (point_data_.coordinates[i] == point_data_.coordinates[j]) {
                    new_page = true;
                }
            }
        }
    }

    ~PDFRendering() {
        if (pdf_) {
            HPDF_Free(pdf_);
        }
    }
private:
    void createA3Page();

    void setFontSize(HPDF_Page page, double size);

    static void drawCircle(HPDF_Page page, double x, double y, double radius);

    static void draw_line(HPDF_Page page, HPDF_REAL x1, HPDF_REAL y1, HPDF_REAL x2, HPDF_REAL y2);

public:
    void generatePDF(const std::string& filename) {
        createA3Page();
        HPDF_Page page = HPDF_GetCurrentPage(pdf_);
        size_t size_point_tbl = point_tbl.size();
        int blank = 0;
        int totalPage = 1;
        if(new_page){
            size_point_tbl/=2;
            totalPage = 2;
        }

        for(auto page_list = 0; page_list < totalPage; page_list++) {
            if(page_list == 1){
                createA3Page();
                page = HPDF_GetCurrentPage(pdf_);
                size_point_tbl = point_tbl.size();
            }
            for (; blank < size_point_tbl; blank++) {


                //test Удалить в конце, не рисуется
                if (false) {
                    drawCircle(page, point_data_.coordinates[point_tbl[blank] - 1].first,
                               point_data_.coordinates[point_tbl[blank] - 1].second, cfm_data_.diameter / 2.0f);
                }

                // отрисовка круга
                if (false) {
                    drawCircle(page, point_data_.coordinates[point_tbl[blank] - 1].first,
                               point_data_.coordinates[point_tbl[blank] - 1].second, zgt_data_.diameter / 2.0f);
                }

                if (false) {
                    //Центральный отверстие
                    drawCircle(page, point_data_.coordinates[point_tbl[blank] - 1].first,
                               point_data_.coordinates[point_tbl[blank] - 1].second,
                               zgt_data_.center_hole_diameter / 2.0f);
                }

                // Индексы не изменяются
                if (false) {
                    double xy = zgt_data_.mounting_holes[0];
                    double yz = zgt_data_.mounting_holes[1];
                    yz = yz > 0 ? yz : -yz;
                    drawCircle(page, point_data_.coordinates[point_tbl[blank] - 1].first + xy,
                               point_data_.coordinates[point_tbl[blank] - 1].second + yz,
                               zgt_data_.mounting_holes[2] / 2.0f);

                    xy = zgt_data_.mounting_holes[3];
                    yz = zgt_data_.mounting_holes[4];
                    yz = yz > 0 ? yz : -yz;
                    drawCircle(page, point_data_.coordinates[point_tbl[blank] - 1].first + xy,
                               point_data_.coordinates[point_tbl[blank] - 1].second + yz,
                               zgt_data_.mounting_holes[5] / 2.0f);
                }

                size_t size = tbl_data_[blank].measurements.size();
                auto a0 = (tbl_data_[blank].measurements[0][0] + tbl_data_[blank].measurements[0][1]) / 2.0;
                auto an =
                        (tbl_data_[blank].measurements[size - 1][0] + tbl_data_[blank].measurements[size - 1][1]) / 2.0;
                auto aDelta = (an - a0) / size;

                std::vector<double> marks;
                for (int i = 0; i < size; i++) {
                    auto asred = (tbl_data_[blank].measurements[i][0] + tbl_data_[blank].measurements[i][1]) / 2.0;
                    marks.push_back(asred + aDelta);
                }

                //Рисуем линии главные
                double radius = cfm_data_.diameter / 2.0;
                size_t i = 0;
                for (double angle: marks) {
                    angle = 4.71239 - angle;

                    double sinAngle = sin(angle);
                    double cosAngle = cos(angle);
                    HPDF_REAL x1, x2, y1, y2;

                    x1 = point_data_.coordinates[point_tbl[blank] - 1].first +
                         (radius - cfm_data_.digital_tick[0]) * cosAngle;
                    y1 = point_data_.coordinates[point_tbl[blank] - 1].second +
                         (radius - cfm_data_.digital_tick[0]) * sinAngle;

                    if (i == 0) { // Если это первая итерация, сместить верхнюю линию на 90 градусов
                        double shift_angle = angle + 1.5708; // Смещение на 90 градусов вправо
                        double center_offset = cfm_data_.digital_tick[2] / 2; // Размер центра

                        x1 = point_data_.coordinates[point_tbl[blank] - 1].first +
                             (radius - cfm_data_.digital_tick[0]) * cosAngle;
                        y1 = point_data_.coordinates[point_tbl[blank] - 1].second +
                             (radius - cfm_data_.digital_tick[0]) * sinAngle;

                        x1 -= center_offset * cos(shift_angle); // Смещение x координаты
                        y1 -= center_offset * sin(shift_angle); // Смещение y координаты

                        x2 = x1 + (cfm_data_.digital_tick[0] - cfm_data_.digital_tick[2]) *
                                  cos(shift_angle); // Правая часть линии
                        y2 = y1 + (cfm_data_.digital_tick[0] - cfm_data_.digital_tick[2]) *
                                  sin(shift_angle); // Правая часть линии
                    } else {
                        x2 = point_data_.coordinates[point_tbl[blank] - 1].first +
                             (radius - cfm_data_.digital_tick[2]) * cosAngle;
                        y2 = point_data_.coordinates[point_tbl[blank] - 1].second +
                             (radius - cfm_data_.digital_tick[2]) * sinAngle;
                    }

                    // Верняя линия
                    HPDF_Page_SetLineWidth(page, cfm_data_.digital_tick[3]);

                    draw_line(page, x2, y2, x1, y1);

                    // Нижняя линия
                    x1 = point_data_.coordinates[point_tbl[blank] - 1].first + radius * cos(angle);
                    y1 = point_data_.coordinates[point_tbl[blank] - 1].second + radius * sin(angle);
                    x2 = point_data_.coordinates[point_tbl[blank] - 1].first +
                         (radius - cfm_data_.digital_tick[2]) * cosAngle;
                    y2 = point_data_.coordinates[point_tbl[blank] - 1].second +
                         (radius - cfm_data_.digital_tick[2]) * sinAngle;

                    if (i == 0) {
                        x2 = point_data_.coordinates[point_tbl[blank] - 1].first +
                             (radius - cfm_data_.digital_tick[0]) * cosAngle;
                        y2 = point_data_.coordinates[point_tbl[blank] - 1].second +
                             (radius - cfm_data_.digital_tick[0]) * sinAngle;
                    }

                    HPDF_Page_SetLineWidth(page, cfm_data_.digital_tick[1]);
                    draw_line(page, x2, y2, x1, y1);

                    i++;
                }

                auto radius2 = radius - cfm_data_.digital_tick[0] / 2;
                setFontSize(page, cfm_data_.digit_height);
                for (i = 0; i < marks.size(); i++) {
                    HPDF_REAL angle = 4.71239 - marks[i];
                    HPDF_REAL x = point_data_.coordinates[point_tbl[blank] - 1].first +
                                  (radius2 - cfm_data_.digital_tick[0]) * cos(angle);
                    HPDF_REAL y = point_data_.coordinates[point_tbl[blank] - 1].second +
                                  (radius2 - cfm_data_.digital_tick[0]) * sin(angle);

                    HPDF_Page_BeginText(page);
                    HPDF_Page_MoveTextPos(page, x, y - 2);
                    HPDF_Page_ShowText(page, std::to_string(i).c_str());
                    HPDF_Page_EndText(page);
                }

                size_t size_tick_mask = cfm_data_.tick_mask.size() - 1;
                for (size_t k = 0; k < marks.size() - 1; k++) {
                    double angle1 = 4.71239 - marks[k];
                    double angle2 = 4.71239 - marks[k + 1];
                    double angle_diff = (angle2 - angle1) / double(size_tick_mask);

                    for (size_t j = 1; j <= size_tick_mask; j++) {
                        double angle = angle1 + j * angle_diff;
                        HPDF_REAL x1 = point_data_.coordinates[point_tbl[blank] - 1].first + radius * cos(angle);
                        HPDF_REAL y1 = point_data_.coordinates[point_tbl[blank] - 1].second + radius * sin(angle);
                        HPDF_REAL x2, y2;
                        HPDF_REAL height_line;

                        if (cfm_data_.tick_mask[j] == '1') {
                            height_line = cfm_data_.small_tick[0];
                        }
                        if (cfm_data_.tick_mask[j] == '2') {
                            height_line = cfm_data_.big_tick[0];
                        }
                        if (cfm_data_.tick_mask[j] == '3') {
                            height_line = cfm_data_.digital_tick[0];
                        }

                        x2 = point_data_.coordinates[point_tbl[blank] - 1].first + (radius - height_line) * cos(angle);
                        y2 = point_data_.coordinates[point_tbl[blank] - 1].second + (radius - height_line) * sin(angle);

                        draw_line(page, x1, y1, x2, y2);
                    }
                }
                HPDF_SaveToFile(pdf_, filename.c_str());
            }
        }
    }

private:
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
