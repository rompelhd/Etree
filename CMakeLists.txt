cmake_minimum_required(VERSION 3.0)
project(etree_project)

add_executable(etree src/etree.cpp src/update.cpp)

file(GLOB HEADER_FILES include/*.h)

target_include_directories(etree PRIVATE include)

find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

target_link_libraries(etree ${CURL_LIBRARIES})
