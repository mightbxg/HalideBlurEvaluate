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

# generate libs
set(OUTPUT_LIBS_DIR ${CMAKE_CURRENT_BINARY_DIR}/libs)
set(OUTPUT_LIBS "")
macro(add_lib_gen TARGET)
    set(OUTPUT_DIR ${OUTPUT_LIBS_DIR}/${TARGET})
    add_custom_command(OUTPUT ${OUTPUT_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_DIR})
    add_custom_target(${TARGET}
        COMMAND blur_generator -g halide_blur -o ${OUTPUT_DIR} target=${TARGET}
        DEPENDS blur_generator ${OUTPUT_DIR}
        COMMENT "generating halide_blur for ${TARGET}")
    list(APPEND OUTPUT_LIBS ${TARGET})
endmacro()

add_lib_gen(host)
add_lib_gen(x86-64-linux-opencl)
add_lib_gen(arm-64-android)
add_lib_gen(arm-64-android-opencl)
add_custom_target(genlibs ALL
    DEPENDS ${OUTPUT_LIBS})
