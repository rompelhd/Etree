#include "../include/update.hpp"
#include <curl/curl.h>
#include <regex>
#include <set>
#include <vector>

std::string url = "https://raw.githubusercontent.com/rompelhd/Etree/main/version";
std::string tags_url = "https://github.com/rompelhd/Etree/tags";
std::string ver_url = "https://github.com/rompelhd/Etree/releases/tag/";

void checkupdate(const std::map<std::string, std::string>& translations) {
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
                    std::cout << translations.at("version_need_update") << std::endl;
                    CURL *curl_tags;
                    curl_tags = curl_easy_init();
                    if (curl_tags) {
                        curl_easy_setopt(curl_tags, CURLOPT_URL, tags_url.c_str());
                        curl_easy_setopt(curl_tags, CURLOPT_WRITEFUNCTION, WriteCallback);
                        curl_easy_setopt(curl_tags, CURLOPT_WRITEDATA, &buffer);
                        res = curl_easy_perform(curl_tags);
                        if (res == CURLE_OK) {
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
                                std::cout << "Number of outdated versions: " << outdated_count << " ==>" << " [";
                                for (size_t i = 0; i < outdated_versions.size() - 1; ++i) {
                                    std::cout << outdated_versions[i] << ", ";
                                }
                                std::cout << outdated_versions.back() << "]" << std::endl;

                                for (const auto& version : outdated_versions) {
                                    std::string tag_url = ver_url + version;
                                    std::cout << "Changes in [ " << version << " ]" << std::endl;
                                    CURL *curl_tag;
                                    std::string buffer_tag;
                                    curl_tag = curl_easy_init();
                                    if (curl_tag) {
                                        curl_easy_setopt(curl_tag, CURLOPT_URL, tag_url.c_str());
                                        curl_easy_setopt(curl_tag, CURLOPT_WRITEFUNCTION, WriteCallback);
                                        curl_easy_setopt(curl_tag, CURLOPT_WRITEDATA, &buffer_tag);
                                        res = curl_easy_perform(curl_tag);
                                        if (res == CURLE_OK) {
                                            std::regex li_regex("<li>(.+?)<\\/li>");
                                            std::smatch match;
                                            std::string::const_iterator search_start(buffer_tag.cbegin());
                                            while (std::regex_search(search_start, buffer_tag.cend(), match, li_regex)) {
                                                std::string li_content = match[1];
                                                std::cout << "Content: " << li_content << std::endl;
                                                search_start = match.suffix().first;
                                            }

                                            std::regex commits_regex("<a href=\".*\">\\s*(\\d+) commits\\s*</a>");
                                            if (std::regex_search(buffer_tag, match, commits_regex)) {
                                                std::cout << "Commits until the last version: " << match[1] << std::endl;
                                            }
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
}
