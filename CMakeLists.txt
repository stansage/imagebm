cmake_minimum_required(VERSION 2.8)

project(imagebm)
add_executable(${PROJECT_NAME} "main.c" "qdbmp/qdbmp.h" "qdbmp/qdbmp.c")
