
# main build
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} sprite_project animation raylib)
target_include_directories(${PROJECT_NAME} PUBLIC include)
