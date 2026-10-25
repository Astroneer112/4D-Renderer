cmake_minimum_required(VERSION 3.0)

project(fourdrenderer)

add_executable(fourdrenderer main.cpp)

install(TARGETS fourdrenderer RUNTIME DESTINATION bin)
