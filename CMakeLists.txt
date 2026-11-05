cmake_minimum_required(VERSION 3.10)

project(fourdrenderer)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED sdl2)

add_executable(fourdrenderer main.cpp)

target_include_directories(fourdrenderer PRIVATE ${SDL2_INCLUDE_DIRS})
target_compile_options(fourdrenderer PRIVATE ${SDL2_CFLAGS_OTHER})

target_compile_options(fourdrenderer PRIVATE -march=native -ffast-math)

target_link_libraries(fourdrenderer PRIVATE ${SDL2_LIBRARIES} atomic)

install(TARGETS fourdrenderer RUNTIME DESTINATION bin)
