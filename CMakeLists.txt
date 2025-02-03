cmake_minimum_required(VERSION 3.10)
project(kamikaze)

# Set C Standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Dependencies
# set(RAYLIB_VERSION 5.5) # Change this to the version you want to use
set(RAYLIB_VERSION "master")
include(FetchContent)
# check if RAYLIB_VERSION is a number
if(RAYLIB_VERSION MATCHES "^[0-9.]+$")
    find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED

    if (NOT raylib_FOUND) # If there's none, fetch and build raylib
        FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
        )
        FetchContent_GetProperties(raylib)
        if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
            set(FETCHCONTENT_QUIET NO)
            FetchContent_MakeAvailable(raylib)
            set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        endif()

    endif()
else()
    # download the latest version of raylib
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG ${RAYLIB_VERSION} 
    )
    FetchContent_GetProperties(raylib)
    set(FETCHCONTENT_QUIET NO)
    FetchContent_MakeAvailable(raylib)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
endif()  

add_executable(kamikaze main.c)
target_link_libraries(kamikaze raylib)