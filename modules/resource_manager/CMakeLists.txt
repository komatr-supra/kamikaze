add_library(resource_module STATIC resource_manager.c)

target_include_directories(resource_module PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(resource_module PRIVATE raylib)