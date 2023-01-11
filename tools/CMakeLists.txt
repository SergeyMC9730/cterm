cmake_minimum_required(VERSION 3.0.0)
project(tools VERSION 1.1.0)

add_executable(get_module_name get_module_name.c)
add_executable(get_module_description get_module_description.c)
add_executable(get_module_version get_module_version.c)

target_link_libraries(get_module_name dl)
target_link_libraries(get_module_description dl)
target_link_libraries(get_module_version dl)