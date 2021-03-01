cmake_minimum_required(VERSION 3.10)

project(BlurGenerator)

find_package(Halide REQUIRED)

add_executable(blur_generator blur_generator.cpp)
target_link_libraries(blur_generator Halide::Generator)

add_halide_library(halide_blur FROM blur_generator)

add_executable(run_blur run_blur.cpp)
target_link_libraries(run_blur PRIVATE
    halide_blur Halide::ImageIO)
target_compile_features(run_blur PRIVATE cxx_std_17)
