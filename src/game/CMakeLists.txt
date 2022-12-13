cmake_minimum_required(VERSION 3.15)

add_library(AirGame game_scene.cpp
                    game_scene.h
                    game_player_controller.cpp
                    game_player_controller.h)
target_include_directories(AirGame PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AirGame reactphysics3d EnTT::EnTT)