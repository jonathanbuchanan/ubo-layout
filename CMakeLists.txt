cmake_minimum_required(VERSION 2.6)
project(ubo-layout)
add_executable(ubo-layout main.c) 

install(TARGETS ubo-layout DESTINATION bin)
