#include "../include/etree.hpp"
#include "../include/json.hpp"
#include "../include/update.hpp"
#include "../include/config_utils.hpp"
#include <algorithm>
#include <vector>

class Tree {
private:
    void exploreDirectory(const fs::path &path, const string &prefix) {
        try {
        } catch (const std::filesystem::filesystem_error &ex) {
            //cerr << "Error: " << ex.what() << endl;
        }
    }

    bool isBinaryFile(const string &filename) {
    ifstream file(filename, ios::binary);
    if (!file) {
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

void walk(const string &directory, const string &prefix, bool parameAFlag, bool parameDFlag, bool parameFFlag, bool parameNFlag) {
        try {
            vector<fs::directory_entry> entries;

            if (parameDFlag) {
                for (const auto &entry : fs::directory_iterator(directory)) {
                    if (!entry.is_directory()) {
                        continue;
                    }

                    if (!parameAFlag && entry.path().filename().string()[0] == '.') {
                        continue;
                    }

                    entries.push_back(entry);
                }
            } else {
                for (const auto &entry : fs::directory_iterator(directory)) {
                    if (parameDFlag && !entry.is_directory()) {
                        continue;
                    }
                    if (!parameAFlag && entry.path().filename().string()[0] == '.') {
                        continue;
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
                if (entry.is_symlink()) {
                    fs::path target = fs::read_symlink(entry.path());
                    fs::path absoluteTarget = fs::canonical(target);
                    cout << prefix << pointers[0];
                    cout << " " << entry.path().c_str() << " ⇒ " << absoluteTarget.c_str() << endl;
                    files++;
                    continue;
                }

                if (fs::is_character_file(entry.path()) || fs::is_block_file(entry.path())) {
                    cout << prefix << pointers[0];
                    cout << " " << entry.path().filename().string() << endl;
                    files++;
                    continue;
                }

                // Cout if the entry have been a directory

                if (entry.is_directory() && !isBinaryFile(entry.path().string())) {
                    for (const auto &tuple : iconsWithExtensions) {
                        const std::string &icon = std::get<0>(tuple);
                        const std::string &extension = std::get<1>(tuple);
                        const std::string &color = std::get<2>(tuple);

                        if (extension.empty()) {
                            cout << prefix << pointers[0];

                            if (!parameNFlag && !color.empty()) {
                                cout << color;
                            }

                            if (parameFFlag) {
                               cout << std::get<0>(tuple) << "./" << relativePath(entry.path()) << Colours::endColour << endl;
                               dirs++;

                               if (!parameDFlag) {
                                   walk(entry.path(), prefix + pointers[1], parameAFlag, parameDFlag, parameFFlag, parameNFlag);
                               }

                               break;
                            }

                            if (!parameFFlag) {
                                cout << std::get<0>(tuple) << entry.path().filename().string();

                                if (!parameNFlag) {
                                    cout << Colours::endColour;
                                }

                                cout << endl;

                                dirs++;

                                if (!parameDFlag) {
                                    walk(entry.path(), prefix + pointers[1], parameAFlag, parameDFlag, parameFFlag, parameNFlag);
                                }

                                break;
                            }
                        }
                    }
                } else {
                    if (!parameDFlag) {
                        bool extensionChecked = false;
                        for (const auto &tuple : iconsWithExtensions) {
                            const std::string &icon = std::get<0>(tuple);
                            const std::string &extension = std::get<1>(tuple);
                            const std::string &color = std::get<2>(tuple);

                            // Cout if the colors have been checked with the extension

                            if (!extension.empty() && entry.path().extension() == extension) {
                                cout << prefix << pointers[0];
                                if (!parameNFlag && !color.empty()) {
                                    cout << color;
                                }

                                if (!parameFFlag) {
                                    cout << icon << entry.path().filename().string() << Colours::endColour << endl;
                                    files++;
                                }

                                if (parameFFlag) {
                                    cout << icon << "./" << relativePath(entry.path()) << Colours::endColour << endl;
                                    files++;
                                }
                                extensionChecked = true;
                                break;
                            }
                        }

                        if (!extensionChecked) {
                            bool isBinary = isBinaryFile(entry.path().string());
                            bool hasExtension = !entry.path().extension().empty();

                            for (const auto &tuple : iconsWithExtensions) {
                                const std::string &icon = std::get<0>(tuple);
                                const std::string &extension = std::get<1>(tuple);
                                const std::string &color = std::get<2>(tuple);

                                if (!extension.empty() && entry.path().extension() == extension) {
                                    cout << prefix << pointers[0];
                                    if (!color.empty()) {
                                        cout << color;
                                    }
                                    cout << icon << entry.path().filename().string() << Colours::endColour << endl;
                                    extensionChecked = true;
                                    files++;
                                    break;
                                }
                            }
                            if (!extensionChecked && hasExtension && !isBinary) {
                                for (const auto &tuple : iconsWithExtensions) {
                                    const std::string &icon = std::get<0>(tuple);
                                    if (std::get<1>(tuple).empty()) {
                                        cout << prefix << pointers[0] << icon << entry.path().filename().string() << endl;
                                        files++;
                                        break;
                                    }
                                }

                            // Cout if binary has been checked

                            } else if (!extensionChecked && isBinary) {
                                  for (const auto &tuple : iconsWithExtensions) {
                                      const std::string &icon = std::get<0>(tuple);
                                      const std::string &color = std::get<2>(tuple);
                                      const std::tuple<std::string, std::string, std::string> &sixthElement = iconsWithExtensions[19];
                                      std::string sixthIcon = std::get<0>(sixthElement);

                                      if (std::get<1>(tuple).empty()) {
                                          if (parameFFlag) {
                                              if (parameNFlag) {
                                                  cout << prefix << pointers[0] << sixthIcon << "./" << relativePath(entry.path()) << endl;
                                              } else {
                                                  cout << prefix << pointers[0] << Colours::greenColour << sixthIcon << "./" << relativePath(entry.path()) << Colours::endColour << endl;
                                              }
                                          } else {
                                              if (parameNFlag) {
                                                  cout << prefix << pointers[0] << sixthIcon << entry.path().filename().string() << endl;
                                              } else {
                                                  cout << prefix << pointers[0] << Colours::greenColour << sixthIcon << entry.path().filename().string() << Colours::endColour << endl;
                                              }
                                          }
                                          break;
                                      }
                                  }

                            } else if (!extensionChecked) {
                                cout << prefix << pointers[0] << entry.path().filename().string() << endl;
                            }
                            files++;
                        }
                    }
                }

            } catch (const std::filesystem::filesystem_error &ex) {
                //cerr << "Error: " << ex.what() << endl;
            }
        }
    } catch (const std::filesystem::filesystem_error &ex) {
        //cerr << "Error: " << ex.what() << endl;
    }
};

};

void printDirectory(const std::string& directory, bool parameNFlag) {
    std::string output;
    if (!parameNFlag) {
        if (fs::equivalent(fs::path(directory), fs::current_path())) {
            output = Colours::blueColour + "." + Colours::endColour + '\n';
            dirs++;
        } else {
            output = Colours::blueColour + " " + directory + Colours::endColour + '\n';
            dirs++;
        }
    } else {
        if (fs::equivalent(fs::path(directory), fs::current_path())) {
            output = ".\n";
            dirs++;
        } else {
            output = " " + directory;
            output.push_back('\n');
            dirs++;
        }
    }
    std::cout << output << std::flush;
}

void showHelp(const std::map<std::string, std::string>& translations) {
    std::cout << translations.at("usage") << std::endl;
    std::cout << translations.at("list_op") << std::endl;
    std::cout << translations.at("-a-fun") << std::endl;
    std::cout << translations.at("-d-fun") << std::endl;
    std::cout << translations.at("-f-fun") << std::endl;
    std::cout << translations.at("list_grap") << std::endl;
    std::cout << translations.at("-n-fun") << std::endl;
    std::cout << translations.at("list_mis") << std::endl;
    std::cout << translations.at("-ver-fun") << std::endl;
    std::cout << translations.at("help_option") << std::endl;
}

void parame_f(const std::string& directory, Tree& tree, const std::map<std::string, std::string>& translations, bool parameFFlag) {
    printDirectory(directory, false);

    tree.walk(directory, "", false, false, parameFFlag, false);

    std::cout << "\n" << Colours::greenColour << dirs << Colours::endColour << " " << Colours::blueColour << translations.at("directories_trn") << Colours::endColour << ", " << Colours::greenColour << files << Colours::purpleColour << " " << translations.at("files_trn") << Colours::endColour << std::endl;
}

void parame_a(const std::string& directory, Tree& tree, const std::map<std::string, std::string>& translations, bool parameAFlag) {
    printDirectory(directory, false);

    tree.walk(directory, "", parameAFlag, false, false, false);

    std::cout << "\n" << Colours::greenColour << dirs << Colours::endColour << " " << Colours::blueColour << translations.at("directories_trn") << Colours::endColour << ", " << Colours::greenColour << files << Colours::purpleColour << " " << translations.at("files_trn") << Colours::endColour << std::endl;
}

void parame_d(const std::string& directory, Tree& tree, const std::map<std::string, std::string>& translations, bool parameDFlag) {
    printDirectory(directory, false);

    tree.walk(directory, "", false, parameDFlag, false, false);

    std::cout << "\n" << Colours::greenColour << tree.getDirsCount() << Colours::endColour << " " << Colours::blueColour << translations.at("directories_trn") << Colours::endColour << std::endl;
}

void parame_n(const std::string& directory, Tree& tree, const std::map<std::string, std::string>& translations, bool parameNFlag) {
    printDirectory(directory, parameNFlag);

    tree.walk(directory, "", false, false, false, parameNFlag);

    std::cout << "\n" << dirs << " " << translations.at("directories_trn") << ", " << files << " " << translations.at("files_trn") << std::endl;
}

void param(int argc, char *argv[]) {
    std::string directory = ".";

    bool showHelpFlag = false;
    bool parameNFlag = false;
    bool parameAFlag = false;
    bool showVerFlag = false;
    bool parameDFlag = false;
    bool parameFFlag = false;
    bool parameLFlag = false;

    std::string languageCode = ConfigLoad(Paths::getEtreeFolderPath() + "etree.conf");
    std::string languageFilePath = Paths::getLanguageFilePath(languageCode);
    std::map<std::string, std::string> translations = loadTranslations(languageFilePath);
    std::set<std::string> recognizedArgs = {"--help", "--version", "-a", "-L", "-d", "-f", "-n"};

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--help") {
            showHelpFlag = true;
        } else if (arg == "--version") {
            showVerFlag = true;
        } else if (arg == "-a") {
            parameAFlag = true;
        } else if (arg == "-L") {
            parameLFlag = true;
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

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg[0] == '-' && recognizedArgs.find(arg) == recognizedArgs.end()) {
            std::cerr << "Etree: Argumento no reconocido: " << arg << std::endl;
            showHelp(translations);
            exit(1);
        }
    }

    if (!directory.empty()) {
        if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory)) {
            std::cerr << directory << " [Error al abrir el directorio]" << std::endl;
            exit(1);
        }
    }

    if (showHelpFlag) {
        showHelp(translations);
        return;
    }

    if (showVerFlag) {
        std::cout << "Etree " << translations.at("version") << " by Rompelhd" << std::endl;
        checkupdate(translations);
        return;
    }

    if (parameNFlag) {
        Tree tree;
        parame_n(directory, tree, translations, parameNFlag);
        return;
    }

    if (parameAFlag) {
        Tree tree;
        parame_a(directory, tree, translations, parameAFlag);
        return;
    }

    if (parameDFlag) {
        Tree tree;
        parame_d(directory, tree, translations, parameDFlag);
        return;
    }

    if (parameFFlag) {
        Tree tree;
        parame_f(directory, tree, translations, parameFFlag);
        return;
    }

    printDirectory(directory, false);
    Tree tree;
    tree.walk(directory, "", parameAFlag, parameDFlag, parameFFlag, parameNFlag);
    std::cout << "\n" << Colours::greenColour << dirs << Colours::endColour << " " << Colours::blueColour << translations.at("directories_trn") << Colours::endColour << ", " << Colours::greenColour << files << Colours::purpleColour << " " << translations.at("files_trn") << Colours::endColour << std::endl;
}

int main(int argc, char *argv[]) {
    param(argc, argv);
    return 0;
}
