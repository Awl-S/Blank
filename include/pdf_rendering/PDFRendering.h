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
        pdf_ = HPDF_New(NULL, NULL);
        if(!pdf_){
            throw std::runtime_error("Не удалось создать PDF документ.");
        }
        font_ = HPDF_GetFont(pdf_, HPDF_LoadTTFontFromFile(pdf_, "/home/orys/Desktop/setup/font/arial.ttf", HPDF_TRUE), "CP1251");
        if(!font_){
            throw std::runtime_error("Не удалось загрузить шрифт.");
        }
//        page_ = HPDF_AddPage(pdf_);

//        HPDF_Page_SetSize(page_, HPDF_PAGE_SIZE_A3, HPDF_PAGE_PORTRAIT);
//        HPDF_Page_SetHeight(page_, 420);
//        HPDF_Page_SetWidth(page_, 297);
    }

    ~PDFRendering() {
        if (pdf_) {
            HPDF_Free(pdf_);
        }
    }

    void createA3Page() {
        HPDF_Page page = HPDF_AddPage(pdf_);
        HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A3, HPDF_PAGE_PORTRAIT);
    }

    void setFontSize(HPDF_Page page, float size) {
        HPDF_Page_SetFontAndSize(page, font_, size);
        HPDF_Page_SetHeight(page, 420);
        HPDF_Page_SetWidth(page, 297);
    }

    float mmToPoint(float mm) {
        return mm * (72.0f / 25.4f);
    }

    void setLineWidth(HPDF_Page page, float width) {
        HPDF_Page_SetLineWidth(page, width);
    }

    void drawCircle(HPDF_Page page, float x, float y, float radius) {
        HPDF_Page_Circle(page, x, y, radius);
        HPDF_Page_Stroke(page);
    }

    void rotatePoint(float x, float y, float angle, float centerX, float centerY, float &outX, float &outY) {
        float s = sin(angle);
        float c = cos(angle);

        // Перемещение точки к началу координат
        x -= centerX;
        y -= centerY;

        // Применение поворота
        outX = x * c - y * s;
        outY = x * s + y * c;

        // Возвращение точки в исходную систему координат
        outX += centerX;
        outY += centerY;
    }

    void generatePDF(const std::string& filename) {
        createA3Page();
        HPDF_Page page = HPDF_GetCurrentPage(pdf_);

        setFontSize(page, 12.0);
        setLineWidth(page, 0.5);
        //test Удалить в конце, не рисуется
        drawCircle(page, point_data_.coordinates[0].first, point_data_.coordinates[0].second, cfm_data_.diameter / 2.0f);

        // отрисовка круга
        if(false){
            drawCircle(page, point_data_.coordinates[0].first, point_data_.coordinates[0].second, zgt_data_.diameter / 2.0f);
        }

        if(false){
            //Центральный отверстие
            drawCircle(page, point_data_.coordinates[0].first, point_data_.coordinates[0].second, zgt_data_.center_hole_diameter / 2.0f);
        }

        // Индексы не изменяются
        if(false){
            double xy = zgt_data_.mounting_holes[0];
            double yz = zgt_data_.mounting_holes[1];
            yz = yz > 0 ? yz : -yz;
            drawCircle(page, point_data_.coordinates[0].first+xy, point_data_.coordinates[0].second + yz, zgt_data_.mounting_holes[2] / 2.0f);

            xy  = zgt_data_.mounting_holes[3];
            yz = zgt_data_.mounting_holes[4];
            yz = yz > 0 ? yz : -yz;
            drawCircle(page, point_data_.coordinates[0].first+xy, point_data_.coordinates[0].second + yz, zgt_data_.mounting_holes[5] / 2.0f);
        }

        size_t size = tbl_data_[0].measurements.size();
        auto a0 = (tbl_data_[0].measurements[0][0] + tbl_data_[0].measurements[0][1]) / 2.0;
        auto an = (tbl_data_[0].measurements[size - 1][0] + tbl_data_[0].measurements[size - 1][1]) / 2.0;
        auto aDelta = (an - a0) / size;

        std::vector<double>marks;
        for(int i = 0; i < size; i++){
            auto asred = (tbl_data_[0].measurements[i][0] + tbl_data_[0].measurements[i][1]) / 2.0;
            marks.push_back(asred + aDelta);
        }

        for(auto i : marks){
            std::cout << i << ", ";
        }

        //Рисуем линии главные
        double radius = cfm_data_.diameter / 2.0;
        size_t i = 0;
        for (double angle : marks) {
            angle = 4.71239 - angle;

            double sinAngle = sin(angle);
            double cosAngle = cos(angle);
            float x1, x2, y1, y2;

            x1 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[0]) * cosAngle;
            y1 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[0]) * sinAngle;

           if (i == 0) { // Если это первая итерация, сместить верхнюю линию на 90 градусов
               double shift_angle = angle + 1.5708; // Смещение на 90 градусов вправо
               double center_offset = cfm_data_.digital_tick[2] / 2; // Размер центра

               x1 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[0]) * cosAngle;
               y1 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[0]) * sinAngle;

               x1 -= center_offset * cos(shift_angle); // Смещение x координаты
               y1 -= center_offset * sin(shift_angle); // Смещение y координаты

               x2 = x1 + (cfm_data_.digital_tick[0] - cfm_data_.digital_tick[2]) * cos(shift_angle); // Правая часть линии
               y2 = y1 + (cfm_data_.digital_tick[0] - cfm_data_.digital_tick[2]) * sin(shift_angle); // Правая часть линии
            } else {
                x2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
                y2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;
            }

            // Верняя линия
            HPDF_Page_SetLineWidth(page, cfm_data_.digital_tick[3]);
            HPDF_Page_MoveTo(page, x2, y2);
            HPDF_Page_LineTo(page, x1, y1);
            HPDF_Page_Stroke(page);

            HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf_, "Helvetica", NULL), cfm_data_.format[0]);

            // Нижняя линия
            x1 = point_data_.coordinates[0].first + radius * cos(angle);
            y1 = point_data_.coordinates[0].second + radius * sin(angle);
            x2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
            y2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;

            if(i == 0) {
                x2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[0]) * cosAngle;
                y2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[0]) * sinAngle;
            }

            HPDF_Page_SetLineWidth(page, cfm_data_.digital_tick[1]);
            HPDF_Page_MoveTo(page, x2, y2);
            HPDF_Page_LineTo(page, x1, y1);
            HPDF_Page_Stroke(page);

//            for(int e = 0; e < cfm_data_.tick_mask.size()-1; e++) {
//                float z1, f1, z2, f2;
//                z1 = point_data_.coordinates[0].first + radius * cos(angle);
//                f1 = point_data_.coordinates[0].second + radius * sin(angle);
//                z2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
//                f2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;
//                if (cfm_data_.tick_mask[e] == '1') {
//                    z2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
//                    f2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;
//                }
//                if(cfm_data_.tick_mask[e] == '2'){
//                    z2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
//                    f2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;
//                }
//                if(cfm_data_.tick_mask[e] == '3'){
//                    z2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
//                    f2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;
//                }
//
//                HPDF_Page_SetLineWidth(page, cfm_data_.digital_tick[1]);
//                HPDF_Page_MoveTo(page, z2, f2);
//                HPDF_Page_LineTo(page, z1, f1);
//                HPDF_Page_Stroke(page);
//            }

            i++;
        }

        radius = cfm_data_.diameter / 2.0;
        HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf_, "Helvetica", NULL), cfm_data_.digit_height);
        for(i = 0; i < marks.size(); i++) {
            HPDF_REAL angle = 4.71239 - marks[i];
            HPDF_REAL x = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[0]) * cos(angle);
            HPDF_REAL y = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[0]) * sin(angle);

            HPDF_Page_BeginText(page);
            HPDF_Page_MoveTextPos(page, x-4, y-4 );
            HPDF_Page_ShowText(page, std::to_string(i).c_str());
            HPDF_Page_EndText(page);
        }

//        double max_mark = *std::max_element(cfm_data_.marks.begin(), cfm_data_.marks.end());
//        std::stringstream max_mark_ss;
//        max_mark_ss << std::fixed << std::setprecision(cfm_data_.format[1]) << max_mark;
//        std::string max_mark_str = max_mark_ss.str();
//        size_t max_mark_length = max_mark_str.length();
//
//        HPDF_Page_SetFontAndSize(page, HPDF_GetFont(pdf_, "Helvetica", NULL), cfm_data_.digit_height);
//        auto radius1 = radius /2 + cfm_data_.digital_tick[0];
//        auto xDelta = cfm_data_.digital_tick[2]/3.f;
//        auto yDelta = cfm_data_.digital_tick[2]/3.f;
//        double total_length_mm = max_mark_length * cfm_data_.digit_height * 0.5; // Здесь используется значение 0.5 для пропорции ширины символа к его высоте
//
//        for (size_t i = 0; i < marks.size(); ++i) {
//            double angle = 4.71239 - marks[i];
//            std::cout << marks[i]*180/3.14159265358979323846 << std::endl;
//            if(marks[i]*180/3.14159265358979323846 > 180){
//                xDelta +=total_length_mm/4.f;
////                yDelta -=0.5;
//            }
//            double x = point_data_.coordinates[0].first + radius1 * cos(angle);
//            double y = point_data_.coordinates[0].second + radius1 * sin(angle);
//
//            std::stringstream ss;
//            ss << std::fixed << std::setprecision(cfm_data_.format[1]) << cfm_data_.marks[i];
//
//            HPDF_Page_BeginText(page);
//            HPDF_Page_TextOut(page, x-xDelta, y-yDelta, ss.str().c_str());
//            HPDF_Page_EndText(page);
//        }

        std::cout << cfm_data_.tick_mask << std::endl;

//        for(int e = 0; e < cfm_data_.tick_mask.size()-1; e++) {
//            float z1, f1, z2, f2;
//            z1 = point_data_.coordinates[0].first + radius * cos(angle);
//            f1 = point_data_.coordinates[0].second + radius * sin(angle);
//            z2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
//            f2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;
//            if (cfm_data_.tick_mask[e] == '1') {
//                z2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
//                f2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;
//            }
//            if(cfm_data_.tick_mask[e] == '2'){
//                z2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
//                f2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;
//            }
//            if(cfm_data_.tick_mask[e] == '3'){
//                z2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
//                f2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;
//            }
//
//            HPDF_Page_SetLineWidth(page, cfm_data_.digital_tick[1]);
//            HPDF_Page_MoveTo(page, z2, f2);
//            HPDF_Page_LineTo(page, z1, f1);
//            HPDF_Page_Stroke(page);
//        }

        HPDF_SaveToFile(pdf_, filename.c_str());
    }

private:
    HPDF_Page page_;
    HPDF_Doc pdf_;
    HPDF_Font font_;

    cfm cfm_data_;
    zgt zgt_data_;
    nbr nbr_data_;
    point point_data_;
    std::vector<tbl> tbl_data_;
};

#endif //BLANK_PDFRENDERING_H
