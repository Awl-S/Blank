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
