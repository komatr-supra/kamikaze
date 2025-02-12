cmake_minimum_required(VERSION 3.16)
project(kamikaze LANGUAGES C)

# Set C Standard (11 is multithread support - can be useful at multiplayer)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# to prevent create function instead of calling it(can be invisible for compiler and then he can create a new one) X
add_compile_options(-Wimplicit-function-declaration)

set(RAYLIB_BUILD_EXAMPLES OFF CACHE BOOL "Don't build raylib examples" FORCE)

# modules
add_subdirectory(modules)
# Dependencies - sorry for this, folder is presistent, you can delete cmake folder and the project will build without internet connection, or download a repo and got everything
add_subdirectory(external/raylib-5.5)

# simple resources copy - TODO better way and streucture? - still ok for start
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/resources ${CMAKE_BINARY_DIR}/kamikaze/resources
)

add_subdirectory(src)

# just copy folder before build - maybe not required?
add_dependencies(${PROJECT_NAME} copy_resources)