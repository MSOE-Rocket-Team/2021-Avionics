cmake_minimum_required(VERSION 3.13)

project(pff VERSION 0.03)
add_library(pff pff.c)
set_target_properties(pff PROPERTIES VERSION ${PROJECT_VERSION})
