cmake_minimum_required(VERSION 3.16)
project(kamikaze LANGUAGES C)
# Set C Standard (11 is multithread support - can be useful at multiplayer)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# to prevent create function instead of calling them(can be invisible for him => create a new) X
add_compile_options(-Wimplicit-function-declaration)
# Dependencies - sorry, this folder is presistent, you can delete cmake folder and the project will build without internet connection :D
add_subdirectory(external/raylib-5.5)
# simple resources copy - TODO better?
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/resources
)

add_executable(kamikaze main.c)
target_link_libraries(kamikaze raylib)
add_dependencies(kamikaze copy_resources)