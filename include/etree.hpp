#ifndef ETREE_HPP
#define ETREE_HPP

#include <map>
#include <string>
#include <cstdlib>

namespace Colours {
    const std::string greenColour = "\033[1;32m";
    const std::string endColour = "\033[0m";
    const std::string redColour = "\033[1;31m";
    const std::string blueColour = "\033[1;34m";
    const std::string yellowColour = "\033[1;33m";
    const std::string purpleColour = "\033[1;35m";
    const std::string turquoiseColour = "\033[1;36m";
    const std::string grayColour = "\033[1;37m";
}

namespace Paths {
    inline std::string getConfigFolderPath() {
        std::string homeDir = getenv("HOME");
        return homeDir + "/.config/";
    }

    inline std::string getEtreeFolderPath() {
        return getConfigFolderPath() + "etree/";
    }

    inline std::string getLanguageFilePath(const std::string& languageCode) {
        return getEtreeFolderPath() + "locales/" + languageCode + ".json";
    }
}

#endif
