cmake_minimum_required(VERSION 3.19)
project(coroutine C)

set(CMAKE_C_STANDARD 99)

add_executable(coroutine coroutine.c app.c)