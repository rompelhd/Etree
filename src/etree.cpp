#include "../include/etree.hpp"
#include "../include/json.hpp"
#include <curl/curl.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

class Tree {
private:
    void exploreDirectory(const fs::path &path, const string &prefix) {
        try {
        } catch (const std::filesystem::filesystem_error &ex) {
            cerr << "Error: " << ex.what() << endl;
        }
    }

    bool isBinaryFile(const string &filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening the file: " << filename << endl;
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
    size_t getDirsCount() const {
        return dirs;
    }

    size_t getFilesCount() const {
        return files;
    }

    void walk(const string &directory, const string &prefix, bool showHiddenFiles, bool showOnlyDirectories, bool parameDFlag, bool parameFFlag) {
        try {
            vector<fs::directory_entry> entries;

            if (parameFFlag) {
              for (const auto &entry : fs::directory_iterator(directory)) {
                  if (entry.is_directory()) {
                    string relativePath = fs::relative(entry.path(), fs::current_path()).string();
                    walk(entry.path(), prefix + "│lol   ", showHiddenFiles, showOnlyDirectories, parameDFlag, parameFFlag);
                    cout << relativePath;
                  } else {
                      string relativePath = fs::relative(entry.path(), fs::current_path()).string();
                      cout << relativePath;
                  }
              }

            }

            if (parameDFlag) {
                for (const auto &entry : fs::directory_iterator(directory)) {
                    if (!entry.is_directory()) {
                        continue;
                    }

                    if (!showHiddenFiles && entry.path().filename().string()[0] == '.') {
                        continue;
                    }

                    entries.push_back(entry);
                }
            } else {
                for (const auto &entry : fs::directory_iterator(directory)) {
                    if (showOnlyDirectories && !entry.is_directory()) {
                        continue;
                    }
                    if (!showHiddenFiles && entry.path().filename().string()[0] == '.') {
                        continue;
                    }

                    if (!showOnlyDirectories && parameDFlag) {
                        walk(entry.path(), prefix + "  ", showHiddenFiles, showOnlyDirectories, parameDFlag, parameFFlag);
                    }

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
                    cout << prefix << pointers[0] << Colours::blueColour << iconsWithExtensions[0].first << entry.path().filename().string() << Colours::endColour << endl;
                    dirs++;
                    if (!showOnlyDirectories) {
                        walk(entry.path(), prefix + pointers[1], showHiddenFiles, showOnlyDirectories, parameDFlag, parameFFlag);
                    }
                } else {
                    if (!showOnlyDirectories) {
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
                            cout << prefix << pointers[0] << Colours::greenColour << iconsWithExtensions[3].first << entry.path().filename().string() << Colours::endColour << endl;
                        } else if (!extensionChecked) {
                            cout << prefix << pointers[0] << entry.path().filename().string() << endl;
                        }

                        files++;
                    }
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
        std::string translatedDirectories = translations.at("directories_trn");
        std::string translatedFiles = translations.at("files_trn");

        std::cout << "\n" << Colours::greenColour << dirs << Colours::endColour << " " << Colours::blueColour << translatedDirectories << Colours::endColour << ", " << Colours::greenColour << files << Colours::purpleColour << " " << translatedFiles << Colours::endColour << std::endl;
    }
};

void showVer(const std::map<std::string, std::string>& translations) {
    std::cout << "Etree " << translations.at("version") << " by Rompelhd" << std::endl;
}

//void updating() {
//}

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

void parame_f(const std::string& directory, Tree& tree, const std::map<std::string, std::string>& translations, bool parameFFlag) {
    std::string currentDirectory = fs::current_path().string();

    if (fs::equivalent(fs::path(directory), fs::current_path())) {
        std::cout << Colours::blueColour << "." << Colours::endColour << std::endl;
    } else {
        std::cout << Colours::blueColour << directory << Colours::endColour << std::endl;
    }

    tree.walk(directory, "", false, false, false, parameFFlag);

}

void parame_a(const std::string& directory, Tree& tree, const std::map<std::string, std::string>& translations, bool showHiddenFiles) {
    std::string currentDirectory = fs::current_path().string();

    if (fs::equivalent(fs::path(directory), fs::current_path())) {
        std::cout << Colours::blueColour << "." << Colours::endColour << std::endl;
    } else {
        std::cout << Colours::blueColour << directory << Colours::endColour << std::endl;
    }

    tree.walk(directory, "", showHiddenFiles, false, false, false);

    size_t dirs = tree.getDirsCount();
    size_t files = tree.getFilesCount();

    std::string translatedDirectories = translations.at("directories_trn");
    std::string translatedFiles = translations.at("files_trn");

    std::cout << "\n" << Colours::greenColour << dirs << Colours::endColour << " " << Colours::blueColour << translatedDirectories << Colours::endColour << ", " << Colours::greenColour << files << Colours::purpleColour << " " << translatedFiles << Colours::endColour << std::endl;

}

void parame_d(const std::string& directory, Tree& tree, const std::map<std::string, std::string>& translations, bool parameDFlag) {
    bool showHiddenFiles = false;
    bool showOnlyDirectories = false;

    std::cout << Colours::blueColour << "." << Colours::endColour << std::endl;

    tree.walk(directory, "", false, showOnlyDirectories, parameDFlag, false);

    std::string translatedDirectories = translations.at("directories_trn");
    std::cout << "\n" << Colours::greenColour << tree.getDirsCount() << Colours::endColour << " " << Colours::blueColour << translatedDirectories << Colours::endColour << std::endl;
}

void parame_n(const std::string& directory, Tree& tree, const std::map<std::string, std::string>& translations) {
    bool showHiddenFiles = false;
    bool showOnlyDirectories = false;
    std::string currentDirectory = fs::current_path().string();

    if (fs::equivalent(fs::path(directory), fs::current_path())) {
        std::cout << Colours::blueColour << "." << Colours::endColour << std::endl;
    } else {
        std::cout << Colours::blueColour << directory << Colours::endColour << std::endl;
    }

    tree.walk(directory, "", showHiddenFiles, false, false, false);

    size_t dirs = tree.getDirsCount();
    size_t files = tree.getFilesCount();

    std::string translatedDirectories = translations.at("directories_trn");
    std::string translatedFiles = translations.at("files_trn");

    std::cout << "\n" << Colours::greenColour << dirs << Colours::endColour << " " << Colours::blueColour << translatedDirectories << Colours::endColour << ", " << Colours::greenColour << files << Colours::purpleColour << " " << translatedFiles << Colours::endColour << std::endl;
}

void param(int argc, char *argv[]) {
    std::string directory = ".";
    std::string languageCode = "es";
    bool showHelpFlag = false;
    bool parameNFlag = false;
    bool parameAFlag = false;
    bool showVerFlag = false;
    bool parameDFlag = false;
    bool parameFFlag = false;

    bool showHiddenFiles = false;
    bool showOnlyDirectories = false;

    std::string languageFilePath = Paths::getLanguageFilePath(languageCode);
    std::map<std::string, std::string> translations = loadTranslations(languageFilePath);

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help") {
            showHelpFlag = true;
        } else if (arg == "--version") {
            showVerFlag = true;
        } else if (arg == "-a") {
            parameAFlag = true;
        } else if (arg == "-d") {
            parameDFlag = true;
        } else if (arg == "-f") {
            parameFFlag = true;
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
        showHelp(translations);
        return;
    }

    if (showVerFlag) {
        showVer(translations);
        checkupdate(translations);
        return;
    }

    if (parameNFlag) {
        Tree tree;
        parame_n(directory, tree, translations);
        return;
    }

    if (parameAFlag) {
        showHiddenFiles = true;
        Tree tree;
        parame_a(directory, tree, translations, showHiddenFiles);
        return;
    }

    if (parameDFlag) {
        Tree tree;
        showOnlyDirectories = true;
        parame_d(directory, tree, translations, parameDFlag);
        return;
    }

    if (parameFFlag) {
        Tree tree;
        parame_f(directory, tree, translations, parameFFlag);
        return;
    }

    if (fs::equivalent(fs::path(directory), fs::current_path())) {
        std::cout << Colours::blueColour << "." << Colours::endColour << std::endl;
    } else {
        std::cout << Colours::blueColour << directory << Colours::endColour << std::endl;
    }

    Tree tree;

    tree.walk(directory, "", showHiddenFiles, showOnlyDirectories, parameDFlag, parameFFlag);
    tree.summary(translations);
}

int main(int argc, char *argv[]) {
    param(argc, argv);
    return 0;
}
