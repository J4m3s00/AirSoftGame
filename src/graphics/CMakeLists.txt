cmake_minimum_required(VERSION 3.15)

add_library(AirGraphics gra_application.cpp
                        gra_genmesh.cpp)
target_include_directories(AirGraphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AirGraphics raylib AirNet)