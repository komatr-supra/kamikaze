# simple resources copy - TODO better way and streucture? - still ok for start
add_custom_target(copy_resources ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/resources ${CMAKE_BINARY_DIR}/resources
)
# main build
file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS *.c)
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} raylib)
target_include_directories(${PROJECT_NAME} PUBLIC include)

# just copy folder before build - maybe not required?
add_dependencies(${PROJECT_NAME} copy_resources)