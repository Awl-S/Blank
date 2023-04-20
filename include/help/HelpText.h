#ifndef BLANK_HELPTEXT_H
#define BLANK_HELPTEXT_H

#include <string>
#include "iostream"
#include "tinyxml2.h"

using namespace tinyxml2;

std::string read_help_text() {
    XMLDocument doc;
    XMLError error = doc.LoadFile("./settings/help.help");

    if (error != XML_SUCCESS) {
        std::cerr << "Не удалось прочитать help.help: " << doc.ErrorName() << std::endl;
        return "";
    }

    XMLNode* help_node = doc.FirstChildElement("help");
    if (!help_node) {
        std::cerr << "Неверная структура help.help." << std::endl;
        return "";
    }

    XMLElement* text_elem = help_node->FirstChildElement("text");
    if (!text_elem) {
        std::cerr << "Не найден текст в help.help." << std::endl;
        return "";
    }

    const char* text = text_elem->GetText();
    return text ? text : "";
}

#endif //BLANK_HELPTEXT_H
