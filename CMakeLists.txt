cmake_minimum_required(VERSION 3.16.3)
project(untitled C)

set(CMAKE_C_STANDARD 90)

add_executable(untitled main.c
        file.c
        ajaj/parse.c
        ajaj/parse.h
        validate.c
        lp.c
        lp.h
        stack.c
        stack.h
        parse.c
        parse.h
        Generals/generals.c
        Generals/generals.h)
