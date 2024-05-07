#include "../include/basic.hpp"
#include "../include/update.hpp"
#include <curl/curl.h>
#include <regex>
#include <set>
#include <vector>

std::string url = "https://raw.githubusercontent.com/rompelhd/Etree/main/version";
std::string tags_url = "https://github.com/rompelhd/Etree/tags";
std::string ver_url = "https://github.com/rompelhd/Etree/releases/tag/";

int getTerminalWidth() {
    winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return w.ws_col;
}

void hightable(int totalWidth, const std::string& version) {
    std::cout << Colours::blueColour << "╭─ Changes in [ " << Colours::yellowColour << version << Colours::blueColour << " ] ";
    for (int i = 26; i < totalWidth - 1; ++i) {
        std::cout << "─";
    }
    std::cout << "╮" << std::endl;
    std::cout << Colours::blueColour << "│";
    for (int i = 1; i < totalWidth - 1; ++i) {
        std::cout << " ";
    }
    std::cout << Colours::blueColour << "│" << std::endl;
}

size_t getRealLength(const std::string& str) {
    size_t length = 0;
    bool inEscape = false;

    for (char c : str) {
        if (c == '\033') {
            inEscape = true;
        } else if (inEscape && c == 'm') {
            inEscape = false;
        } else if (!inEscape) {
            length++;
        }
    }

    return length;
}

void intertable(int totalWidth, const std::string& li_content, int terminalWidth) {
    std::string output = "\033[34m│  [*] \033[33m" + li_content + "\033[0m";
    size_t realLength = getRealLength(output);
    size_t spacesNeeded = totalWidth - realLength;

    std::string additionalSpacesFirst;
    std::string additionalSpacesSecond;

      if (realLength >= terminalWidth) {
        size_t lastSpace = output.rfind(' ', terminalWidth);

        std::string firstLine = output.substr(0, lastSpace);
        std::string secondLine = output.substr(lastSpace + 1);

        size_t firstLineLength = getRealLength(firstLine);
        size_t secondLineLength = getRealLength(secondLine);

        size_t remainingSpacesFirst = totalWidth - firstLineLength + 1;
        for (size_t i = 0; i < remainingSpacesFirst; ++i) {
            additionalSpacesFirst += " ";
        }

        size_t remainingSpacesSecond = totalWidth - secondLineLength - 8;
        for (size_t i = 0; i < remainingSpacesSecond; ++i) {
            additionalSpacesSecond += " ";
        }

        std::cout << firstLine << additionalSpacesFirst << Colours::blueColour << "│" << std::endl;
        std::cout << Colours::blueColour << "│      " << Colours::yellowColour << secondLine << additionalSpacesSecond << Colours::blueColour << "│" << std::endl;


    } else {
        std::cout << Colours::blueColour << output;
        for (int i = 0; i < spacesNeeded; ++i) {
            std::cout << " ";
        }
        std::cout << Colours::blueColour << " │";
    }
}

void lowtable(int totalWidth) {
    std::cout << Colours::blueColour << "│";
    for (int i = 1; i < totalWidth - 1; ++i) {
        std::cout << " ";
    }
    std::cout << "│" << std::endl;

    std::cout << Colours::blueColour << "╰";
    for (int i = 1; i < totalWidth - 1; ++i) {
        std::cout << "─";
    }
    std::cout << "╯" << std::endl;
}

int checkupdate(const std::map<std::string, std::string>& translations) {
    CURL *curl;
    CURLcode res;
    std::string buffer;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    std::string version_translations = translations.at("version");
    int outdated_count = 0;
    std::vector<std::string> outdated_versions;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << translations.at("error_version_download") << curl_easy_strerror(res) << std::endl;
        } else {
            size_t pos = buffer.find("v.");
            if (pos != std::string::npos) {
                std::string download_ver = buffer.substr(pos, 7);
                if (version_translations == download_ver) {
                    std::cout << translations.at("version_is_update") << std::endl;
                } else {
                    //std::cout << translations.at("version_need_update") << std::endl;
                    CURL *curl_tags;
                    curl_tags = curl_easy_init();
                    if (curl_tags) {
                        curl_easy_setopt(curl_tags, CURLOPT_URL, tags_url.c_str());
                        curl_easy_setopt(curl_tags, CURLOPT_WRITEFUNCTION, WriteCallback);
                        curl_easy_setopt(curl_tags, CURLOPT_WRITEDATA, &buffer);
                        res = curl_easy_perform(curl_tags);
                        if (res == CURLE_OK) {
                            int terminalWidth = getTerminalWidth();
                            if (terminalWidth < 85) {
                                std::cout << "Error: Tamaño de terminal no válido." << std::endl;
                                return 1;
                            }

                            std::set<std::string> unique_versions;
                            std::regex version_regex("v\\.[0-9]+\\.[0-9]+\\.[0-9]+");
                            std::smatch match;
                            std::string::const_iterator search_start(buffer.cbegin());
                            while (std::regex_search(search_start, buffer.cend(), match, version_regex)) {
                                std::string version = match.str();
                                unique_versions.insert(version);
                                search_start = match.suffix().first;
                            }
                            for (const auto& version : unique_versions) {
                                if (version_translations != version) {
                                    outdated_versions.push_back(version);
                                    outdated_count++;
                                }
                            }
                            if (!outdated_versions.empty()) {
                                std::cout << Colours::blueColour << "\nNumber of outdated versions: " << Colours::yellowColour << outdated_count << Colours::blueColour << " ==>" << " [";
                                for (size_t i = 0; i < outdated_versions.size() - 1; ++i) {
                                    std::cout << Colours::yellowColour << outdated_versions[i] << Colours::blueColour << ", ";
                                }
                                std::cout << Colours::yellowColour << outdated_versions.back() << Colours::blueColour << "]" << "\n" << Colours::endColour << std::endl;

                                for (const auto& version : outdated_versions) {
                                    std::string tag_url = ver_url + version;
                                    CURL *curl_tag;
                                    std::string buffer_tag;
                                    curl_tag = curl_easy_init();
                                    if (curl_tag) {
                                        curl_easy_setopt(curl_tag, CURLOPT_URL, tag_url.c_str());
                                        curl_easy_setopt(curl_tag, CURLOPT_WRITEFUNCTION, WriteCallback);
                                        curl_easy_setopt(curl_tag, CURLOPT_WRITEDATA, &buffer_tag);
                                        res = curl_easy_perform(curl_tag);
                                        if (res == CURLE_OK) {

                                            hightable(terminalWidth, version);

                                            std::regex li_regex("<li>(.+?)<\\/li>");
                                            std::smatch match;
                                            std::string::const_iterator search_start(buffer_tag.cbegin());
                                            std::string li_content;
                                            while (std::regex_search(search_start, buffer_tag.cend(), match, li_regex)) {
                                                std::string li_content = match[1];

                                                intertable(terminalWidth, li_content, terminalWidth);

                                                search_start = match.suffix().first;
                                            }

                                            lowtable(terminalWidth);

                                        } else {
                                            std::cerr << translations.at("error_tag_download") << curl_easy_strerror(res) << std::endl;
                                        }
                                        curl_easy_cleanup(curl_tag);
                                    }
                                }
                            } else {
                                std::cout << "No outdated versions found." << std::endl;
                            }
                        } else {
                            std::cerr << translations.at("error_tags_download") << curl_easy_strerror(res) << std::endl;
                        }
                        curl_easy_cleanup(curl_tags);
                    }
                }
            } else {
                std::cout << translations.at("no_version_found") << std::endl;
            }
        }
        curl_easy_cleanup(curl);
    }
    curl_global_cleanup();
    return 0;
}
