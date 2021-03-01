cmake_minimum_required(VERSION 3.10)

project(BlurGenerator)

find_package(Halide REQUIRED)

add_executable(blur_generator blur_generator.cpp)
target_link_libraries(blur_generator Halide::Generator)
