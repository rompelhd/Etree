#include "librarys/json.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <map>
#include <set>
#include <iostream>
#include <string>
#include <vector>

using json = nlohmann::json;
using namespace std;
namespace fs = filesystem;

const string greenColour = "\033[1;32m";
const string endColour = "\033[0m";
const string redColour = "\033[1;31m";
const string blueColour = "\033[1;34m";
const string yellowColour = "\033[1;33m";
const string purpleColour = "\033[1;35m";
const string turquoiseColour = "\033[1;36m";
const string grayColour = "\033[1;37m";

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
        std::cerr << "No se pudo abrir el archivo de traducciones" << std::endl;
    }

    return translations;
}

class Tree {
private:
    size_t dirs = 0;
    size_t files = 0;
    std::set<std::string> visited_links;
    vector<string> inner_pointers = { "├── ", "│   " };
    vector<string> final_pointers = { "└── ", "    " };
    std::vector<std::pair<std::string, std::string>> iconsWithExtensions = {
        {" ", ""},
        {" ", ".cpp"},
        {" ", ".py"},
        {" ", ""},
        {" ", ".db"},
        {" ", ""},
        {" ", ".js"},
        {" ", ".zip"},
        {" ", ".jar"},
        {" ", ".php"},
        {" ", ".txt"},
        {"󰌛 ", ".c#"},
        {" ", ".ts"},
        {" ", ".c"},
        {" ", ".rb"},
        {" ", ".css"},
        {" ", ".html"}
    };

    bool isBinaryFile(const string &filename) {
        ifstream file(filename, ios::binary);
        if (!file) {
            cerr << "Error al abrir el archivo: " << filename << endl;
            return false;
        }

        char buffer[4096];
        while (file.read(buffer, sizeof(buffer))) {
            for (size_t i = 0; i < file.gcount(); ++i) {
                if (buffer[i] == '\0') {
                    file.close();
                    return true;
                }
            }
        }

        file.close();
        return false;
    }

    bool checkExtension(const string &file, const vector<string> &extensions) {
        for (const auto &ext : extensions) {
            if (fs::path(file).extension() == ext) {
                return true;
            }
        }
        return false;
    }

public:
    void walk(const string &directory, const string &prefix) {
        try {
            vector<fs::directory_entry> entries;

            for (const auto &entry : fs::directory_iterator(directory)) {
                if (entry.path().filename().string()[0] != '.') {
                    entries.push_back(entry);
                }
            }

            sort(entries.begin(), entries.end(), [](const fs::directory_entry &left, const fs::directory_entry &right) -> bool {
                return left.path().filename() < right.path().filename();
            });

            for (size_t index = 0; index < entries.size(); index++) {
                fs::directory_entry entry = entries[index];
                vector<string> pointers = index == entries.size() - 1 ? final_pointers : inner_pointers;

                try {
                    if (fs::is_character_file(entry.path()) || fs::is_block_file(entry.path())) {
                        cout << "Dispositivo especial: " << entry.path().string() << endl;
                        continue;
                    }

                    if (entry.is_symlink()) {
                        string canonical_path = fs::canonical(entry.path()).string();

                        if (visited_links.find(canonical_path) != visited_links.end()) {
                            continue;
                        }

                        visited_links.insert(canonical_path);
                    }

                    if (entry.is_directory()) {
                        cout << prefix << pointers[0] << blueColour << iconsWithExtensions[0].first << entry.path().filename().string() << endColour << endl;
                        dirs++;
                        walk(entry.path(), prefix + pointers[1]);
                    } else {
                          bool extensionChecked = false;

                          for (const auto &pair : iconsWithExtensions) {
                              const std::string &extension = pair.second;
                              if (!extension.empty() && entry.path().extension() == extension) {
                                  cout << prefix << pointers[0] << pair.first << entry.path().filename().string() << endl;
                                  extensionChecked = true;
                                  break;
                              }
                          }

                          if (!extensionChecked && entry.path().extension().empty() && !isBinaryFile(entry.path().string())) {
                              cout << prefix << pointers[0] << iconsWithExtensions[5].first << entry.path().filename().string() << endl;
                          } else if (!extensionChecked && isBinaryFile(entry.path().string())) {
                              cout << prefix << pointers[0] << greenColour << iconsWithExtensions[3].first << entry.path().filename().string() << endColour << endl;
                          } else if (!extensionChecked) {
                              cout << prefix << pointers[0] << entry.path().filename().string() << endl;
                          }

                          files++;
                      }
                } catch (const std::filesystem::filesystem_error &ex) {
                    cerr << "Error: " << ex.what() << endl;
                }
            }
        } catch (const std::filesystem::filesystem_error &ex) {
            cerr << "Error: " << ex.what() << endl;
        }
    }

    void summary(const std::map<std::string, std::string>& translations) {
        std::string translatedDirectories = translations.at("translated_directories_value");
        std::string translatedFiles = translations.at("translated_files_value");

        std::cout << "\n" << greenColour << dirs << endColour << " " << blueColour << translatedDirectories << endColour << ", " << greenColour << files << endColour << " " << translatedFiles << std::endl;
    }
};

int main(int argc, char *argv[]) {
    std::string localesFolder = "locales";
    std::string languageCode = "es";

    std::map<std::string, std::string> translations = loadTranslations(localesFolder + "/" + languageCode + ".json");

    Tree tree;
    string directory = argc == 1 ? "." : argv[1];

    cout << blueColour << directory << endColour << endl;
    tree.walk(directory, "");
    tree.summary(translations);

    return 0;
}
