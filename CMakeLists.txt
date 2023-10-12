cmake_minimum_required(VERSION 3.0.0)
project(cterm VERSION 1.2.0)

add_subdirectory(cJSON)
include_directories(cJSON)
add_subdirectory(applications)
add_subdirectory(tools)

add_library(cterm cterm.c config_json.c)

add_executable(cterm_loader main.c)

target_compile_options(cterm PRIVATE -Wincompatible-pointer-types)
target_link_libraries(cterm cjson dl)

target_link_libraries(cterm_loader cterm)