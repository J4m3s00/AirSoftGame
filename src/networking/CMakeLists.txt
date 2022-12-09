cmake_minimum_required(VERSION 3.15)

add_library(AirNet net_enet.cpp
                    server/net_server.cpp
                    client/net_client.cpp)
target_include_directories(AirNet PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(AirNet AirGame)