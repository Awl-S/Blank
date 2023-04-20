#ifndef BLANK_PDFRENDERING_H
#define BLANK_PDFRENDERING_H

#include <utility>
#include <stdexcept>
#include <iostream>
#include <cmath>
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

    void drawLetterT(HPDF_Page page, float x, float y, float radius, float angle, float rotation) {
        float centerX = x + radius * cos(angle * 3.14159265359 / 180.0);
        float centerY = y + radius * sin(angle * 3.14159265359 / 180.0);

        float halfWidth = 20; // половина ширины буквы T

        float x1, y1, x2, y2;

        // Верхняя горизонтальная линия
        rotatePoint(centerX - halfWidth, centerY, rotation, centerX, centerY, x1, y1);
        rotatePoint(centerX + halfWidth, centerY, rotation, centerX, centerY, x2, y2);
        HPDF_Page_MoveTo(page, x1, y1);
        HPDF_Page_LineTo(page, x2, y2);
        HPDF_Page_Stroke(page);
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

        std::vector<double>marks;
        size_t size = tbl_data_[0].measurements.size();
        std::cout << tbl_data_[0].model << std::endl;
        auto a0 = (tbl_data_[0].measurements[0][0] + tbl_data_[0].measurements[0][1]) / 2.0;
        auto an = (tbl_data_[0].measurements[size - 1][0] + tbl_data_[0].measurements[size - 1][1]) / 2.0;
        auto aDelta = (an - a0) / size;

        for(int i = 0; i < size; i++){
            auto asred = (tbl_data_[0].measurements[i][0] + tbl_data_[0].measurements[i][1]) / 2.0;
            marks.push_back(asred + aDelta);
        }

        for(auto i : marks){
            std::cout << i << "\t"  << i*57.29577951308<< std::endl;
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

               x1 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
               y1 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;

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

            // Нижняя линия
            x1 = point_data_.coordinates[0].first + radius * cos(angle);
            y1 = point_data_.coordinates[0].second + radius * sin(angle);
            x2 = point_data_.coordinates[0].first + (radius - cfm_data_.digital_tick[2]) * cosAngle;
            y2 = point_data_.coordinates[0].second + (radius - cfm_data_.digital_tick[2]) * sinAngle;

            HPDF_Page_SetLineWidth(page, cfm_data_.digital_tick[1]);
            HPDF_Page_MoveTo(page, x2, y2);
            HPDF_Page_LineTo(page, x1, y1);
            HPDF_Page_Stroke(page);
            i++;
        }


        std::cout << cfm_data_.tick_mask << std::endl;

        for(int i = 0; i < cfm_data_.tick_mask.size()-1; i++) {
//            if (cfm_data_.tick_mask[i] == '1') {
//                std::cout << "1" << std::endl;
//            }
//            if(cfm_data_.tick_mask[i] == '2'){
//                std::cout << "2" << std::endl;
//            }
//            if(cfm_data_.tick_mask[i] == '3'){
//                std::cout << "3" << std::endl;
//            }
        }

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
