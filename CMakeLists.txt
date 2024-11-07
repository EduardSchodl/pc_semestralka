cmake_minimum_required(VERSION 3.29)
project(untitled C)

set(CMAKE_C_STANDARD 90)

add_executable(untitled main.c
        file.c
        parse.c
        parse.h)
