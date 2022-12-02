cmake_minimum_required(VERSION 3.15)
project(AirSoftShooter)

add_subdirectory(deps/raylib)
add_subdirectory(deps/reactphysics)

add_library(Librg librg_compile.c)

add_executable(Game main.cpp)

add_executable(ServerExec server.cpp)

target_link_libraries(Game raylib reactphysics3d Librg)
target_link_libraries(ServerExec Librg)
