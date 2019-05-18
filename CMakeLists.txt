cmake_minimum_required(VERSION 3.14)
project(event LANGUAGES C VERSION 0.1)

set(CMAKE_C_STANDARD 99)

include_directories(.)

add_library(event STATIC event.c event.h)