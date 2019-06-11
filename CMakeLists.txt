cmake_minimum_required(VERSION 3.14)
project(event C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_library(event event.c)