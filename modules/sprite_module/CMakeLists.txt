add_library(sprite_module STATIC sprite.c)

target_include_directories(sprite_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(sprite_module PRIVATE raylib)