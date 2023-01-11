cmake_minimum_required(VERSION 3.0.0)
project(cterm VERSION 1.1.0)

set(BUILD_SHARED 1)
include(cmake-examples/cmake/FileEmbed.cmake)
FileEmbedSetup()
FileEmbedAdd(${CMAKE_SOURCE_DIR}/config.json)
add_subdirectory(cJSON)
include_directories(cJSON)
add_subdirectory(applications)
add_subdirectory(tools)

add_executable(cterm main.c)

target_compile_options(cterm PRIVATE -Wincompatible-pointer-types)
target_link_libraries(cterm cjson ncurses dl file_embed)