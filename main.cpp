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
AirSoft::Scene      gameScene;

Vector3 GetCameraForward()
{
    float forwardX = cosf(yaw) * cosf(pitch);
    float forwardY = sinf(pitch);
    float forwardZ = sinf(yaw) * cosf(pitch);

    Vector3 result = Vector3Normalize({forwardX, forwardY, forwardZ});
    
    return result;
}

Vector3 GetCameraRight()
{
    Vector3 result;
    result.x = -sin(yaw);
    result.y = 0.0f;
    result.z = cos(yaw);
    return Vector3Normalize(result);
}

Vector3 GetCameraUp()
{
    Vector3 result = GetCameraForward();
    Vector3CrossProduct(result, GetCameraRight());
    return result;
}

void UpdateCamera(Camera& camera);
void HandlePlayerMovement(Camera& camera);
void ConnectToServer();
void InitPlayerEntity(); // Call after connect to server
void UpdatePlayerLocation();

int main()
{
    AirSoft::Application app(&gameScene);

    enet_initialize();

    client.SetCurrentScene(&gameScene);
    client.ConnectTo("localhost");

    ConnectToServer();
    InitPlayerEntity();


    while (!WindowShouldClose())
    {
        client.Update();

        HandlePlayerMovement(app.GetPlayerCamera());
        UpdateCamera(app.GetPlayerCamera());
        UpdatePlayerLocation();

        app.Update();
    }

    client.Disconnect();

    enet_deinitialize();
    CloseWindow();
    
    return 0;
}

void UpdateCamera(Camera& camera)
{
    mousePositionDelta = GetMouseDelta();

    yaw += mousePositionDelta.x*mouseSensity*GetFrameTime();
    pitch -= mousePositionDelta.y*mouseSensity*GetFrameTime();


    if (pitch > 89.0f*DEG2RAD) pitch = 89.0f*DEG2RAD;
    else if (pitch < -89.0f*DEG2RAD) pitch = -89.0f*DEG2RAD;


    directionVector = VECTOR_CAST(Vector3)GetCameraForward();


    camera.target = Vector3Add(camera.position, directionVector);
}

void HandlePlayerMovement(Camera& camera)
{
    #define MOVESPEED (movementSpeed * GetFrameTime())

    Vector3 right = Vector3Scale(GetCameraRight(), MOVESPEED);
    Vector3 forward = GetCameraForward();
    forward.y = 0.0f;   
    forward = Vector3Scale(Vector3Normalize(forward), MOVESPEED);

    bool pressed = false;

    if (IsKeyDown(KEY_W))
    {
        camera.position = Vector3Add(camera.position, forward);
        pressed = true;
        //playerBody->setTransform(rp::Transform(playerBody->getTransform().getPosition() + forward, rp::Quaternion::identity()));
    }
    if (IsKeyDown(KEY_S))
    {
        camera.position = Vector3Add(camera.position, Vector3Negate(forward));
        pressed = true;
        //playerBody->setTransform(rp::Transform(playerBody->getTransform().getPosition() - forward, rp::Quaternion::identity()));
    }
    if (IsKeyDown(KEY_A))
    {
        camera.position = Vector3Add(camera.position, Vector3Negate(right));
        pressed = true;
        //playerBody->setTransform(rp::Transform(playerBody->getTransform().getPosition() - right, rp::Quaternion::identity()));
    }
    if (IsKeyDown(KEY_D))
    {
        camera.position = Vector3Add(camera.position, right);
        pressed = true;
        //playerBody->setTransform(rp::Transform(playerBody->getTransform().getPosition() + right, rp::Quaternion::identity()));
    }
    if (IsKeyDown(KEY_SPACE))
    {
        camera.position.y += MOVESPEED;
        pressed = true;
        //playerBody->applyLocalForceAtCenterOfMass({0.0f, 20.0f, 0.0f});
    } 
    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        camera.position.y -= MOVESPEED;
        pressed = true;
    }

    // Update network
    if (pressed)
    {
        client.Send_PlayerMove(VECTOR_CAST(rp::Vector3)camera.position, {0.0f, 0.0f, 0.0f});
    }
}

void ConnectToServer()
{
}

void InitPlayerEntity()
{
}

void UpdatePlayerLocation()
{
}