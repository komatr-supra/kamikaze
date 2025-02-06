add_library(sprite STATIC sprite.c)

target_include_directories(sprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sprite PRIVATE raylib)