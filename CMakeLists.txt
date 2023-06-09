cmake_minimum_required(VERSION 3.25)
project(Blank)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

add_executable(Blank main.cpp src/command_line/CommandLine.cpp include/command_line/CommandLine.h src/conversion/EncodingConverter.cpp include/conversion/EncodingConverter.h include/help/HelpText.h include/config_parser/ConfigParser.h src/config_parser/JSONConfigParser.cpp include/config_parser/JSONConfigParser.h src/config_parser/XMLConfigParser.cpp include/config_parser/XMLConfigParser.h src/config_parser/Parser.h src/config_parser/TXTConfigParser.cpp include/config_parser/TXTConfigParser.h src/file_reader/FileReader.cpp include/file_reader/FileReader.h src/file_reader/TextFileReader.cpp include/file_reader/TextFileReader.h src/file_reader/JsonFileReader.cpp include/file_reader/JsonFileReader.h src/file_reader/XmlFileReader.cpp include/file_reader/XmlFileReader.h include/struct/cfm.h include/struct/tbl.h include/struct/zgt.h include/struct/point_data.h src/pdf_rendering/PDFRendering.cpp include/pdf_rendering/PDFRendering.h include/struct/nbr.h)

#Подключение библиотек

#XML
find_package(tinyxml2 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE tinyxml2::tinyxml2)

#JSON
find_package(nlohmann_json CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME}  PRIVATE nlohmann_json::nlohmann_json)
#Конвертация
find_package(Iconv REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Iconv::Iconv)
#PDF
find_package(unofficial-libharu CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE unofficial::libharu::hpdf)

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "/home/orys/Desktop/setup/")
