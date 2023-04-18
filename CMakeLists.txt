cmake_minimum_required(VERSION 3.25)
project(Blank)

set(CMAKE_CXX_STANDARD 23)

add_executable(Blank main.cpp src/command_line/CommandLine.cpp include/command_line/CommandLine.h src/conversion/file_converter.cpp include/conversion/file_converter.h src/conversion/EncodingConverter.cpp include/conversion/EncodingConverter.h)

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

