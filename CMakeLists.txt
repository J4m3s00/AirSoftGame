cmake_minimum_required(VERSION 3.15)
project(AirSoftShooter)

add_subdirectory(deps/raylib)
add_subdirectory(deps/reactphysics)
add_subdirectory(deps/entt)

add_executable(Game main.cpp client.cpp)

add_executable(ServerExec server.cpp)

target_link_libraries(Game raylib reactphysics3d EnTT::EnTT)
target_link_libraries(ServerExec)
