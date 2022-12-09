cmake_minimum_required(VERSION 3.15)

add_library(AirGame t.cpp)
target_include_directories(AirGame PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AirGame reactphysics3d EnTT::EnTT)