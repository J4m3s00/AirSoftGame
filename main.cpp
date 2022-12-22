#include "air_net.h"
#include "air_graphics.h"

#include <iostream>



template <typename T>
struct NetworkMessage {
    T ID;
    entt::entity Entity;
    std::vector<uint8_t> Data;
};

enum class GameMessage {
    PLAYER_MOVE
};


namespace rp = reactphysics3d;


#define MAX_LIGHTS 4


const float playerHeight = 20;
const float playerRadius = .2f;

float mouseSensity = 0.25f;
float movementSpeed = 50.0f;

float yaw = 0.0f;
float pitch = 0.0f;

Vector2 mousePositionDelta = {0.0f, 0.0f};
Vector3 directionVector = {0.0f, 0.0f, 0.0f};


AirSoft::NetClient  client;

int main()
{
    AirSoft::GameInstance::Instance().CreateScene();    

    AirSoft::Application app(&client);

    enet_initialize();

    client.ConnectTo("104berlin.de");

    while (!WindowShouldClose())
    {
        client.Update();

        app.Update();
    }
    
    client.Disconnect();
    enet_deinitialize();

    return 0;
}