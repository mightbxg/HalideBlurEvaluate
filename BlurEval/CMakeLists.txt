cmake_minimum_required(VERSION 3.10)

project(BlurEval)

if(NOT Blur_DIR)
    set(Blur_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/host")
    if(NOT EXISTS "${Blur_DIR}/halide_blur.h")
        message(FATAL_ERROR "Blur_DIR not set, and the default dir ${Blur_DIR} is invalid!")
    endif()
endif()

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

add_executable(blur_eval blur_eval.cpp)
target_include_directories(blur_eval PRIVATE ${Blur_DIR})
target_link_directories(blur_eval PRIVATE ${Blur_DIR})
target_link_libraries(blur_eval :halide_blur.a ${OpenCV_LIBS} Threads::Threads dl)
