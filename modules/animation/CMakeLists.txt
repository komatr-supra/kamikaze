add_library(animation STATIC animation.c)

target_include_directories(animation PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(animation PRIVATE sprite_module raylib)