add_library(resource_manager STATIC resource_manager.c)

target_include_directories(resource_manager PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(resource_manager PRIVATE raylib)