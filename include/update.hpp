#ifndef UPDATE_HPP
#define UPDATE_HPP

#include <ostream>
#include <iostream>
#include <map>
#include <string>
#include <sys/ioctl.h>
#include <unistd.h>

extern std::string url;

//Curl function
inline size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *buffer) {
    buffer->append(static_cast<char *>(contents), size * nmemb);
    return size * nmemb;
}

int checkupdate(const std::map<std::string, std::string>& translations);

#endif
