#ifndef ETREE_HPP
#define ETREE_HPP

#include <map>
#include <set>
#include <string>
#include <fstream>
#include <cstdlib>
#include <iostream>
#include "../include/basic.hpp"
#include "json.hpp"
#include <filesystem>

using json = nlohmann::json;
using namespace std;
namespace fs = filesystem;

size_t dirs = 0;
size_t files = 0;
std::set<std::string> visited_links;
std::string currentDirectory = fs::current_path().string();
vector<string> inner_pointers = { "├── ", "│   " };
vector<string> final_pointers = { "└── ", "    " };

//RelativePath
inline std::string relativePath(const fs::path& entryPath) {
    return fs::relative(entryPath, fs::current_path()).string();
}

//Translation Upload
std::map<std::string, std::string> loadTranslations(const std::string& filePath) {
    std::map<std::string, std::string> translations;

    std::ifstream file(filePath);
    if (file.is_open()) {
        json data;
        file >> data;

        for (json::iterator it = data.begin(); it != data.end(); ++it) {
            translations[it.value()] = it.key();
        }

        file.close();
    } else {
        std::cerr << "Error in the translation file" << std::endl;
    }

    return translations;
}

//Icons + Extensions + Color
const std::vector<std::tuple<std::string, std::string, std::string>> iconsWithExtensions = {
    {" ", "", Colours::blueColour},
    {" ", ".cpp", ""},
    {" ", ".py", ""},
    {" ", "", Colours::greenColour},
    {" ", ".db", ""},
    {" ", "", ""},
    {" ", ".js", ""},
    {" ", ".zip", ""},
    {" ", ".jar", ""},
    {" ", ".php", ""},
    {" ", ".txt", ""},
    {"󰌛 ", ".cs", ""},
    {" ", ".ts", ""},
    {" ", ".c", ""},
    {" ", ".rb", ""},
    {" ", ".css", ""},
    {" ", ".html", ""},
    {" ", ".json", ""},
    {" ", ".hpp", ""},
    {" ", ".sh", Colours::greenColour},
    {" ", ".rs", ""},
    {" ", ".mp4", ""},
    {" ", ".mov", ""},
    {" ", ".avi", ""},
    {" ", ".mkv", ""},
    {" ", ".flv", ""},
    {" ", ".wmv", ""},
    {" ", ".divx", ""},
    {" ", ".xvid", ""},
    {" ", ".png", ""},
    {" ", ".jpg", ""},
    {" ", ".jpeg", ""},
    {"󰵸 ", ".gif", ""},
    {" ", ".webp", ""},
    {" ", ".tiff", ""},
    {" ", ".bmp", ""},
    {" ", ".heif", ""},
    {" ", ".svg", ""},
    {" ", ".eps", ""},
    {" ", ".psd", ""},
    {"󰨊 ", ".ps1", ""}
};

#endif