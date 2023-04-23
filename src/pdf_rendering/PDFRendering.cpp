#include "../../include/pdf_rendering/PDFRendering.h"

void PDFRendering::createA3Page() {
    HPDF_Page page = HPDF_AddPage(pdf_);
    HPDF_Page_SetSize(page, HPDF_PAGE_SIZE_A3, HPDF_PAGE_PORTRAIT);
}

void PDFRendering::setFontSize(HPDF_Page page, double size) {
    HPDF_Page_SetFontAndSize(page, font_, size);
    HPDF_Page_SetHeight(page, 420);
    HPDF_Page_SetWidth(page, 297);
}

void PDFRendering::drawCircle(HPDF_Page page, double x, double y, double radius) {
    HPDF_Page_Circle(page, x, y, radius);
    HPDF_Page_Stroke(page);
}

void PDFRendering::draw_line(HPDF_Page page, HPDF_REAL x1, HPDF_REAL y1, HPDF_REAL x2, HPDF_REAL y2) {
    HPDF_Page_MoveTo(page, x1, y1);
    HPDF_Page_LineTo(page, x2, y2);
    HPDF_Page_Stroke(page);
}

void PDFRendering::generate_Marks(size_t blank) {
    size_t size = tbl_data_[blank].measurements.size();
    auto a0 = (tbl_data_[blank].measurements[0][0] + tbl_data_[blank].measurements[0][1]) / 2.0;
    auto an =
            (tbl_data_[blank].measurements[size - 1][0] + tbl_data_[blank].measurements[size - 1][1]) / 2.0;
    auto aDelta = (an - a0) / size;

    for (int i = 0; i < size; i++) {
        auto asred = (tbl_data_[blank].measurements[i][0] + tbl_data_[blank].measurements[i][1]) / 2.0;
        marks.push_back(asred + aDelta);
    }
}

void PDFRendering::drawDigitIndices(HPDF_Page page, size_t blank) {
    auto radius = (cfm_data_.diameter / 2.0) - cfm_data_.digital_tick[0] / 2;
    setFontSize(page, cfm_data_.digit_height);
    for (int i = 0; i < marks.size(); i++) {
        HPDF_REAL angle = 4.71239 - marks[i];
        HPDF_REAL x = point_data_.coordinates[point_tbl[blank] - 1].first +
                      (radius - cfm_data_.digital_tick[0]) * cos(angle);
        HPDF_REAL y = point_data_.coordinates[point_tbl[blank] - 1].second +
                      (radius - cfm_data_.digital_tick[0]) * sin(angle);

        std::stringstream ss;
        ss.precision(cfm_data_.format[1]);
        ss << std::fixed << cfm_data_.marks[i];

        HPDF_Page_BeginText(page);
        HPDF_Page_MoveTextPos(page, x-2, y - 2);
        HPDF_Page_ShowText(page, ss.str().c_str());
        HPDF_Page_EndText(page);
    }
}

void PDFRendering::drawMainLines(HPDF_Page page, size_t blank) {
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
}

void PDFRendering::drawAdditionalTicks(HPDF_Page page, size_t blank) {
    auto radius = cfm_data_.diameter / 2.0f;
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
}

void PDFRendering::generatePDF(const std::string &filename) {

    HPDF_Page page;
    size_t size_point_tbl = point_tbl.size();
    int blank = 0;
    int totalPage = 1;
    if(new_page){
        size_point_tbl/=2;
        totalPage = 2;
    }

    for(auto page_list = 0; page_list < totalPage; page_list++) {
            createA3Page();
            page = HPDF_GetCurrentPage(pdf_);
            if(page_list == 1) {
            size_point_tbl = point_tbl.size();
        }
        for (; blank < size_point_tbl; blank++) {
            HPDF_Page_SetLineWidth(page, cfm_data_.digital_tick[1]);

            //test Удалить в конце, не рисуется
            if (false) {
                drawCircle(page, point_data_.coordinates[point_tbl[blank] - 1].first,
                           point_data_.coordinates[point_tbl[blank] - 1].second, cfm_data_.diameter / 2.0f);
            }

            // отрисовка круга
            if (true) {
                drawCircle(page, point_data_.coordinates[point_tbl[blank] - 1].first,
                           point_data_.coordinates[point_tbl[blank] - 1].second, zgt_data_.diameter / 2.0f);
            }

            if (true) {
                //Центральный отверстие
                drawCircle(page, point_data_.coordinates[point_tbl[blank] - 1].first,
                           point_data_.coordinates[point_tbl[blank] - 1].second,
                           zgt_data_.center_hole_diameter / 2.0f);
            }

            // Индексы не изменяются
            if (true) {
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

            generate_Marks(blank);
            drawMainLines(page, blank);

            drawDigitIndices(page, blank);
            drawAdditionalTicks(page, blank);
            marks.clear();
        }
    }
    HPDF_SaveToFile(pdf_, filename.c_str());
}

HPDF_Font PDFRendering::loadFont(const std::string& fontPath) {
    auto font_name = HPDF_LoadTTFontFromFile(pdf_, fontPath.c_str(), HPDF_TRUE);
    auto font = HPDF_GetFont(pdf_, font_name, "CP1251");

    if(!font){
        throw std::runtime_error("Не удалось загрузить шрифт.");
    }

    return font;
}

void PDFRendering::sortTblData() {
    auto compare_position = [](const tbl& a, const tbl& b) {
        return a.position < b.position;
    };

    // Сортировка вектора tbl_data_ с использованием функции сравнения
    std::sort(tbl_data_.begin(), tbl_data_.end(), compare_position);
}

void PDFRendering::populatePointTbl() {
    for(auto i : tbl_data_){
        point_tbl.push_back(i.position);
    }
}

bool PDFRendering::checkForNewPage() {
    for (size_t i = 0; i < point_data_.coordinates.size(); ++i) {
        for (size_t j = i + 1; j < point_data_.coordinates.size(); ++j) {
            if (point_data_.coordinates[i] == point_data_.coordinates[j]) {
                return true;
            }
        }
    }
    return false;
}

PDFRendering::PDFRendering(cfm cfm_data, zgt zgt_data, const std::vector<tbl> &tbl_data, nbr nbr_data, point point_data)
        : cfm_data_(std::move(cfm_data)), zgt_data_(std::move(zgt_data)), tbl_data_(tbl_data),
          nbr_data_(std::move(nbr_data)), point_data_(std::move(point_data)) {

    pdf_ = HPDF_New(nullptr, nullptr);
    if(!pdf_){
        throw std::runtime_error("Не удалось создать PDF документ.");
    }

    font_ = loadFont("/home/orys/Desktop/setup/font/GOST_A.ttf");
    sortTblData();
    populatePointTbl();
    new_page = checkForNewPage();
}

PDFRendering::~PDFRendering() {
    if (pdf_) {
        HPDF_Free(pdf_);
    }
}
