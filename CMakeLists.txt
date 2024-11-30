cmake_minimum_required(VERSION 3.16.3)
project(untitled C)

set(CMAKE_C_STANDARD 90)

add_executable(untitled main.c
        File/file.c
        File/file.h
        Validate/validate.c
        Validate/validate.h
        LProblem/lp.c
        LProblem/lp.h
        Parse/parse.c
        Parse/parse.h
        Generals/generals.c
        Generals/generals.h
        Bounds/bounds.c
        Bounds/bounds.h
        Subject_to/subject_to.c
        Subject_to/subject_to.h
        Objectives/objectives.c
        Objectives/objectives.h
        Objectives/objectives.c
        Objectives/objectives.h
        Section_buffer/section_buffer.c
        Section_buffer/section_buffer.h
)
