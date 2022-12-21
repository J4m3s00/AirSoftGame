cmake_minimum_required(VERSION 3.15)

add_library(AirGraphics gra_application.cpp
                        gra_application.h
                        gra_genmesh.cpp
                        gra_genmesh.h
                        raygui.c
                        ui/gra_console.cpp
                        ui/gra_console.h)
target_include_directories(AirGraphics PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AirGraphics raylib AirNet)