add_library(timer_module STATIC timer.c)

target_include_directories(timer_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(timer_module PRIVATE raylib)