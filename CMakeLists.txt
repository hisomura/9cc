cmake_minimum_required(VERSION 3.10)
project(9cc C)

set(CMAKE_C_STANDARD 99)

add_executable(9cc codegen.c container.c main.c parse.c tokenize.c main.c )
