cmake_minimum_required(VERSION 3.29)
project(hxdp C)

set(CMAKE_C_STANDARD 11)

add_executable(hxdp
        src/dump.c
        src/term.c
        src/hxdp.c
        include/dump.h
        include/hxdp.h
        include/term.h
        src/message.c
        include/message.h
        src/argsparse.c
        include/argsparse.h)
