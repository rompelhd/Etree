#include "librarys/json.hpp"
#include <curl/curl.h>
#include <filesystem>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <set>

using json = nlohmann::json;
using namespace std;
namespace fs = filesystem;

size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *buffer) {
    buffer->append((char *)contents, size * nmemb);
    return size * nmemb;
}

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
        std::cerr << "Error in the translation file" << std::endl;
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
        {" ", ".html"},
        {" ", ".json"},
        {" ", ".hpp"},
        {" ", ".sh"}
    };

    void exploreDirectory(const fs::path &path, const string &prefix) {
        try {
        } catch (const std::filesystem::filesystem_error &ex) {
            cerr << "Error: " << ex.what() << endl;
        }
    }

public:
    size_t getDirsCount() const {
        return dirs;
    }

    size_t getFilesCount() const {
        return files;
    }

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

        std::cout << "\n" << greenColour << dirs << endColour << " " << blueColour << translatedDirectories << endColour << ", " << greenColour << files << purpleColour << " " << translatedFiles << std::endl;
    }

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
};

void showVer(const std::map<std::string, std::string>& translations) {
    std::cout << "Etree " << translations.at("version") << " by Rompelhd" << std::endl;
}

void checkupdate(const std::map<std::string, std::string>& translations) {
    CURL *curl;
    CURLcode res;
    std::string url = "https://github.com/rompelhd/Etree/blob/main/version";
    std::string buffer;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    std::string version_translations = translations.at("version");

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << translations.at("error_version_download") << curl_easy_strerror(res) << std::endl;
        } else {
            std::regex patron("(v\\.\\d+\\.\\d+\\.\\d+)");

            std::smatch coincidencia;
            if (std::regex_search(buffer, coincidencia, patron)) {
                std::string version_descargada = coincidencia[1];

                if (version_translations == version_descargada) {
                    std::cout << translations.at("version_is_update") << std::endl;
                } else {
                    std::cout << translations.at("version_need_update") << std::endl;
                }
            } else {
                std::cout << translations.at("no_version_found") << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
}

void showHelp(const std::map<std::string, std::string>& translations) {
    std::cout << translations.at("usage") << std::endl;
    std::cout << translations.at("display_tree_description") << std::endl;
    std::cout << translations.at("default_directory_description") << std::endl;
    std::cout << translations.at("arguments") << std::endl;
    std::cout << translations.at("help_option") << std::endl;
}

bool disableColorsFlag = false;

void parame_n(const std::string& directory, Tree& tree, const std::map<std::string, std::string>& translations) {
//    std::cout << "-n is on" << std::endl;
    std::string currentDirectory = fs::current_path().string();

    if (fs::equivalent(fs::path(directory), fs::current_path())) {
        std::cout << blueColour << "." << endColour << std::endl;
    } else {
        std::cout << blueColour << directory << endColour << std::endl;
    }

    tree.walk(directory, "");

    size_t dirs = tree.getDirsCount();
    size_t files = tree.getFilesCount();

    std::string translatedDirectories = translations.at("translated_directories_value");
    std::string translatedFiles = translations.at("translated_files_value");

    std::cout << "\n" << greenColour << dirs << endColour << " " << blueColour << translatedDirectories << endColour << ", " << greenColour << files << purpleColour << " " << translatedFiles << std::endl;
}

void param(int argc, char *argv[]) {
    std::string directory = ".";
    std::string localesFolder = "locales";
    std::string languageCode = "es";
    bool showHelpFlag = false;
    bool parameNFlag = false;
    bool showVerFlag = false;

    std::map<std::string, std::string> translations = loadTranslations(localesFolder + "/" + languageCode + ".json");

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help") {
            showHelpFlag = true;
        } else if (arg == "--version") {
            showVerFlag = true;
        } else if (arg == "-n") {
            parameNFlag = true;
            if (i + 1 < argc) {
                directory = argv[i + 1];
                break;
            }
        } else {
            directory = argv[i];
            break;
        }
    }

    if (showHelpFlag) {
        showHelp();
        return;
    }

    if (showVerFlag) {
        showVer(translations);
        checkupdate(translations);
        return;
    }

    if (parameNFlag) {
        translations = loadTranslations(localesFolder + "/" + languageCode + ".json");
        Tree tree;
        parame_n(directory, tree, translations);
        return;
    }

    Tree tree;

     if (!disableColorsFlag) {
        cout << blueColour << directory << endColour << endl;
    } else {
        cout << directory << endl;
    }

    tree.walk(directory, "");
    tree.summary(translations);
}

int main(int argc, char *argv[]) {
    param(argc, argv);
    return 0;
}
