#include "config_utils.hpp"
#include <iostream>
#include <fstream>

std::string ConfigLoad(const std::string& filename) {
    std::ifstream configFile(filename);
    if (!configFile) {
        std::cerr << "Error opening the config file: " << filename << std::endl;
        return "";
    }

    std::string line;
    while (std::getline(configFile, line)) {
        if (line.empty() || line[0] == '#') continue;

        auto pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);
            if (key == "languageCode") {
                return value;
            }
        }
    }
    std::cerr << "Error: 'languageCode' not found in the config file" << std::endl;
    return "en"; // Default language
}
