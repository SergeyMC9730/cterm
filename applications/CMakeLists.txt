cmake_minimum_required(VERSION 3.0.0)
project(cterm_applications VERSION 1.1.0)

add_subdirectory(curl)

add_library(cterm_generic cterm_generic.c)
add_library(cterm_extensions cterm_extensions.c)
add_library(cterm_tests cterm_tests.c)
add_library(cterm_networking cterm_networking.c)
add_library(cterm_user_environment cterm_user_environment.c)
add_library(cterm_pwd pwd.c)
add_library(cterm_ls ls.c)

target_link_libraries(cterm_networking libcurl)