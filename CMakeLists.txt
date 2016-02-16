cmake_minimum_required(VERSION 2.6)
project(ubo-layout)
add_executable(ubo-layout main.c) 
set(CMAKE_C_FLAGS "-g")

install(TARGETS ubo-layout DESTINATION bin)
