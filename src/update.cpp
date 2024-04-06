#include "../include/update.hpp"
#include <curl/curl.h>

std::string url = "https://raw.githubusercontent.com/rompelhd/Etree/main/version";

void checkupdate(const std::map<std::string, std::string>& translations) {
    CURL *curl;
    CURLcode res;
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
            size_t pos = buffer.find("v.");
            if (pos != std::string::npos) {
                std::string download_ver = buffer.substr(pos, 7);
                if (version_translations == download_ver) {
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
