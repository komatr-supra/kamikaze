add_library(animation_module STATIC animation.c)

target_include_directories(animation_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(animation_module PRIVATE sprite_module raylib)