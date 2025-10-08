cmake_minimum_required(VERSION 3.28)
project(DataStructs C)

set(CMAKE_C_STANDARD 11)

add_executable(DataStructs main.c
        Queue.c
        Queue.h
        Node.c
        Node.h)
