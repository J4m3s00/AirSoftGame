cmake_minimum_required(VERSION 3.15)
project(AirSoftShooter)

add_subdirectory(deps/raylib)
add_subdirectory(deps/reactphysics)
add_subdirectory(deps/entt)

add_subdirectory(src/game)
add_subdirectory(src/networking)
add_subdirectory(src/graphics)

add_executable(Game main.cpp)

add_executable(ServerExec server.cpp)

target_link_libraries(Game AirNet AirGame AirGraphics)
target_link_libraries(ServerExec AirNet)
