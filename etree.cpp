#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
namespace fs = filesystem;

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

public:
    void walk(const string &directory, const string &prefix) {
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

            if (entry.is_directory()) {
                cout << prefix << pointers[0] << folder_icon << entry.path().filename().string() << endl;
                dirs++;
                walk(entry.path(), prefix + pointers[1]);
            } else {
                if (entry.path().extension() == ".cpp") {
                    cout << prefix << pointers[0] << cpp_icon << entry.path().filename().string() << endl;
                } else if (entry.path().extension() == ".py") {
                    cout << prefix << pointers[0] << py_icon << entry.path().filename().string() << endl;
                } else if (entry.path().extension() == ".db") {
                    cout << prefix << pointers[0] << db_icon << entry.path().filename().string() << endl;
                } else if (entry.path().extension().empty() && !isBinaryFile(entry.path().string())) {
                    cout << prefix << pointers[0] << " " << entry.path().filename().string() << endl;
                } else if (isBinaryFile(entry.path().string())) {
                    cout << prefix << pointers[0] << binary_icon << entry.path().filename().string() << endl;
                } else {
                    cout << prefix << pointers[0] << entry.path().filename().string() << endl;
                }
                files++;
            }
        }
    }

    void summary() {
        cout << "\n" << dirs << " Directorios, " << files << " Ficheros" << endl;
    }
};

int main(int argc, char *argv[]) {
    Tree tree;
    string directory = argc == 1 ? "." : argv[1];

    cout << directory << endl;
    tree.walk(directory, "");
    tree.summary();

    return 0;
}
