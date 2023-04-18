#include <utility>

#include "../../include/conversion/EncodingConverter.h"

EncodingConverter::EncodingConverter(std::string  inputDir, const std::string& outputDir)
        : inputDirectory(std::move(inputDir)), outputDirectory(outputDir), useTempDirectory(outputDir.empty()) {
    loadSettingsFromXml(), filesConverted = 0, filesProcessed = 0;
}

void EncodingConverter::loadSettingsFromXml() {
    tinyxml2::XMLDocument doc;
    doc.LoadFile("/home/orys/CLionProjects/Blank/include/xml/settings.xml");

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
            if (std::find(fileExtensions.begin(), fileExtensions.end(), entry.path().extension()) != fileExtensions.end()) {
                convertFile(entry.path());
            }
        }
    }
}

void EncodingConverter::convertFile(const std::filesystem::path& filepath) {
    std::ifstream inFile(filepath, std::ios::binary);
    std::string content((std::istreambuf_iterator<char>(inFile)), std::istreambuf_iterator<char>());

    std::string utf8Content = iconvConvert(content, sourceEncoding, targetEncoding);

    std::filesystem::path outputPath = useTempDirectory ? std::filesystem::temp_directory_path() / filepath.filename() : outputDirectory / filepath.filename();

    std::ofstream outFile(outputPath, std::ios::binary);
    outFile.write(utf8Content.data(), utf8Content.size());
    outFile.close();

    if (useTempDirectory) {
        std::filesystem::copy(outputPath, filepath, std::filesystem::copy_options::overwrite_existing);
        std::filesystem::remove(outputPath);
    }
    filesConverted++;

}

std::string EncodingConverter::iconvConvert(const std::string& input, const std::string& fromCharset, const std::string& toCharset) {
    iconv_t cd = iconv_open(toCharset.c_str(), fromCharset.c_str());
    if (cd == (iconv_t) -1) {
        perror("iconv_open");
        exit(1);
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
            perror("iconv");
            iconv_close(cd);
            exit(1);
        }
    }

    iconv_close(cd);
    outBuf.resize(outBuf.size() - outLeft);

    return std::string(outBuf.begin(), outBuf.end());
}

void EncodingConverter::printStats() {
    std::cout << "Files processed: " << filesProcessed << std::endl;
    std::cout << "Files converted: " << filesConverted << std::endl;
}