add_executable(Eclectic
    CLion/main.c
    Eclectic/crc8.c
    Eclectic/crc16.c
    Eclectic/crc32.c
    Eclectic/list.c
    Eclectic/memory.c
    Eclectic/misc.c
    Eclectic/random.c
)
cmake_minimum_required(VERSION 3.28.3)
include_directories(Eclectic .)
project(Eclectic C)
set(CMAKE_C_STANDARD 23)
