#include <utility>

#include "../../include/conversion/EncodingConverter.h"
/**
@brief Конструктор класса EncodingConverter
@param inputDir Входной каталог
@param outputDir Выходной каталог
Конструктор класса EncodingConverter принимает два параметра: inputDir (входной каталог) и outputDir (выходной каталог).
Значение inputDir передается в качестве rvalue (std::move), а outputDir передается как const reference.
Кроме того, конструктор инициализирует переменные-члены класса:
inputDirectory - с помощью std::move(inputDir) перемещается входной каталог в переменную-член inputDirectory;
outputDirectory - инициализируется переданным выходным каталогом outputDir;
useTempDirectory - инициализируется значением outputDir.empty() - если выходной каталог не задан, то флаг useTempDirectory устанавливается в true.
Далее, в конструкторе вызывается функция loadSettingsFromXml(), которая загружает настройки из XML-файла.
После этого, переменные-члены filesConverted и filesProcessed инициализируются значением 0.
*/
EncodingConverter::EncodingConverter(std::string  inputDir, const std::string& outputDir)
        : inputDirectory(std::move(inputDir)), outputDirectory(outputDir), useTempDirectory(outputDir.empty()) {
    loadSettingsFromXml(), filesConverted = 0, filesProcessed = 0;
}

void EncodingConverter::loadSettingsFromXml() {
    tinyxml2::XMLDocument doc;
    doc.LoadFile("./settings/settings.xml");

    tinyxml2::XMLElement* root = doc.FirstChildElement("settings");
    if (!root) {
        std::cerr << "Invalid settings.xml format." << std::endl;
        exit(1);
    }

    // Load file extensions
    tinyxml2::XMLElement* extElem = root->FirstChildElement("extensions");
    if (extElem) {
        tinyxml2::XMLElement* ext = extElem->FirstChildElement("ext");
        while (ext) {
            fileExtensions.push_back(ext->GetText());
            ext = ext->NextSiblingElement("ext");
        }
    }

    // Load source and target encodings
    sourceEncoding = root->FirstChildElement("sourceEncoding")->GetText();
    targetEncoding = root->FirstChildElement("targetEncoding")->GetText();
}

void EncodingConverter::convert() {
    std::filesystem::path inputPath(inputDirectory);
    convertDirectory(inputPath);
    printStats();

}

void EncodingConverter::convertDirectory(const std::filesystem::path& dir) {
    for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) {
        if (entry.is_regular_file()) {
            filesProcessed++;
            std::filesystem::path relativePath = std::filesystem::relative(entry, inputDirectory);
            std::filesystem::path outputPath = useTempDirectory ? std::filesystem::temp_directory_path() / relativePath : outputDirectory / relativePath;

            if (std::find(fileExtensions.begin(), fileExtensions.end(), entry.path().extension()) != fileExtensions.end()) {
                convertFile(entry.path());
            } else if (!outputDirectory.empty()) {
                // Если файл имеет другое расширение и outputDirectory указана, копируем его
                std::filesystem::create_directories(outputPath.parent_path());
                std::filesystem::copy_file(entry, outputPath, std::filesystem::copy_options::overwrite_existing);
            }
        }
    }
}

bool EncodingConverter::iconvConvert(const std::string& input, std::string& output, const std::string& fromCharset, const std::string& toCharset) {
    iconv_t cd = iconv_open(toCharset.c_str(), fromCharset.c_str());
    if (cd == (iconv_t)-1) {
        perror("iconv_open");
        return false;
    }

    std::vector<char> inBuf(input.begin(), input.end());
    size_t inLeft = inBuf.size();
    size_t outLeft = inLeft * 4;
    std::vector<char> outBuf(outLeft);

    char* inPtr = inBuf.data();
    char* outPtr = outBuf.data();

    while (inLeft > 0) {
        size_t result = iconv(cd, &inPtr, &inLeft, &outPtr, &outLeft);
        if (result == (size_t)-1) {
            iconv_close(cd);
            return false;
        }
    }

    iconv_close(cd);
    outBuf.resize(outBuf.size() - outLeft);

    output.assign(outBuf.begin(), outBuf.end());
    return true;
}

void EncodingConverter::convertFile(const std::filesystem::path& filepath) {
    std::ifstream inFile(filepath, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

    std::filesystem::path relativePath = std::filesystem::relative(filepath, inputDirectory);
    std::filesystem::path outputPath = useTempDirectory ? std::filesystem::temp_directory_path() / relativePath : outputDirectory / relativePath;
    std::filesystem::create_directories(outputPath.parent_path());

    if (!is_utf8(content)) {
        std::string utf8Content;
        if (iconvConvert(content, utf8Content, sourceEncoding, targetEncoding)) {
            content = utf8Content;
            filesConverted++;
        } else {
            std::cerr << "Error converting file '" << filepath << "': Invalid or incomplete multibyte or wide character" << std::endl;
        }
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    outFile.write(content.data(), content.size());
    outFile.close();

    if (useTempDirectory) {
        std::filesystem::copy(outputPath, filepath, std::filesystem::copy_options::overwrite_existing);
        std::filesystem::remove(outputPath);
    }
}

void EncodingConverter::printStats() {
    std::cout << "Files processed: " << filesProcessed << std::endl;
    std::cout << "Files converted: " << filesConverted << std::endl;
}

bool EncodingConverter::is_utf8(const std::string &str) {
    size_t length = str.size();
    for (size_t i = 0; i < length; ) {
        if ((str[i] & 0x80) == 0) {
            i += 1;
        } else if ((str[i] & 0xE0) == 0xC0 && i + 1 < length && (str[i + 1] & 0xC0) == 0x80) {
            i += 2;
        } else if ((str[i] & 0xF0) == 0xE0 && i + 2 < length && (str[i + 1] & 0xC0) == 0x80 && (str[i + 2] & 0xC0) == 0x80) {
            i += 3;
        } else if ((str[i] & 0xF8) == 0xF0 && i + 3 < length && (str[i + 1] & 0xC0) == 0x80 && (str[i + 2] & 0xC0) == 0x80 && (str[i + 3] & 0xC0) == 0x80) {
            i += 4;
        } else {
            return false;
        }
    }
    return true;
}