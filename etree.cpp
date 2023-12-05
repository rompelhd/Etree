#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

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

class Tree {
private:
    size_t dirs = 0;
    size_t files = 0;
    vector<string> inner_pointers = { "├── ", "│   " };
    vector<string> final_pointers = { "└── ", "    " };
    string folder_icon = " ";
    string cpp_icon = " ";
    string py_icon = " ";
    string binary_icon = " ";
    string db_icon = " ";
    string empty_icon = " ";

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
                if (entry.path().filename().string()[0] != '.') {;
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
                        cout << prefix << pointers[0] << blueColour << folder_icon << entry.path().filename().string() << endColour << endl;
                        dirs++;
                        walk(entry.path(), prefix + pointers[1]);
                    } else {
                        vector<string> extensions = { ".cpp", ".py", ".db" };
                        if (checkExtension(entry.path().string(), extensions)) {
                            if (entry.path().extension() == ".cpp") {
                                cout << prefix << pointers[0] << cpp_icon << entry.path().filename().string() << endl;
                            } else if (entry.path().extension() == ".py") {
                                cout << prefix << pointers[0] << py_icon << entry.path().filename().string() << endl;
                            } else if (entry.path().extension() == ".db") {
                                cout << prefix << pointers[0] << db_icon << entry.path().filename().string() << endl;
                            }
                        } else if (entry.path().extension().empty() && !isBinaryFile(entry.path().string())) {
                            cout << prefix << pointers[0] << empty_icon << entry.path().filename().string() << endl;
                        } else if (isBinaryFile(entry.path().string())) {
                            cout << prefix << pointers[0] << greenColour << binary_icon << entry.path().filename().string() << endColour << endl;
                        } else {
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

    void summary() {
        cout << "\n" << greenColour << dirs << endColour << " Directorios, " << greenColour << files << endColour << " Ficheros" << endl;
    }

private:
    std::set<std::string> visited_links;
};

int main(int argc, char *argv[]) {
    Tree tree;
    string directory = argc == 1 ? "." : argv[1];

    cout << blueColour << directory << endColour << endl;
    tree.walk(directory, "");
    tree.summary();

    return 0;
}
