cmake_minimum_required(VERSION 3.0.0)
project(applications VERSION 1.1.0)

add_library(cterm_generic cterm_generic.c)
add_library(cterm_extensions cterm_extensions.c)
add_library(cterm_tests cterm_tests.c)