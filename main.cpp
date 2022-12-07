#include "reactphysics3d/reactphysics3d.h"

#include <iostream>

#include "raylib.h"
#include "rlgl.h"
#include "raymath.h"

#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"



#include "client.h"

#include <entt/entt.hpp>



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

#define VECTOR_CAST(type) *(type*)&


#define MAX_LIGHTS 4


Vector3 floorSize = {100.0f, 1.0f, 100.0f};
float radius = 10.0f;
const float playerHeight = 20;
const float playerRadius = .2f;

float mouseSensity = 0.25f;
float movementSpeed = 50.0f;

float yaw = 0.0f;
float pitch = 0.0f;

Vector2 mousePositionDelta = {0.0f, 0.0f};
Vector3 directionVector = {0.0f, 0.0f, 0.0f};
Camera3D camera = {0};

NetClient client;

entt::registry worldScene;
entt::entity playerEntity;

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

void UpdateCamera();
void HandlePlayerMovement();
void ConnectToServer();
void InitPlayerEntity(); // Call after connect to server
void UpdatePlayerLocation();

int main()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1270, 720, "Air Soft");
    NetInit();

    client.ConnectTo();


    ConnectToServer();
    InitPlayerEntity();

    
    camera.position = { 0.0f, 50.0f, 0.0f };
    camera.target = { 30.0f, 50.0f, 30.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetCameraMode(camera, CAMERA_CUSTOM);
    SetTargetFPS(60);
    DisableCursor();

    Shader shader = LoadShaderFromMemory(
#include "lighting_vs.h"
,
#include "lighting_fs.h"
    );

    shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(shader, "viewPos");

    Model sphereModel = LoadModelFromMesh(GenMeshSphere(radius, 32, 32));
    Model floorModel = LoadModelFromMesh(GenMeshCube(floorSize.x, floorSize.y, floorSize.z));

    sphereModel.materials[0].shader = shader;
    floorModel.materials[0].shader = shader;

    int ambientLoc = GetShaderLocation(shader, "ambient");
    Vector4 ambient = {0.1f, 0.1f, 0.1f, 0.1f};
    SetShaderValue(shader, ambientLoc, &ambient.x, SHADER_UNIFORM_VEC4);


    Light lights[MAX_LIGHTS] = { 0 };
    lights[0] = CreateLight(LIGHT_DIRECTIONAL, Vector3{ -2000, 1000, -2000 }, Vector3Zero(), WHITE, shader);
    //lights[1] = CreateLight(LIGHT_POINT, Vector3{ 20, 10, 20 }, Vector3Zero(), RED, shader);
    //lights[2] = CreateLight(LIGHT_POINT, Vector3{ -20, 10, 20 }, Vector3Zero(), GREEN, shader);
    //lights[3] = CreateLight(LIGHT_POINT, Vector3{ 20, 10, -20 }, Vector3Zero(), BLUE, shader);


    while (!WindowShouldClose())
    {
        client.Update();

        HandlePlayerMovement();
        UpdateCamera();
        UpdatePlayerLocation();

        
        float cameraPos[3] = { camera.position.x, camera.position.y, camera.position.z };
        SetShaderValue(shader, shader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);


        
        for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(shader, lights[i]);


        BeginDrawing(); 
            ClearBackground(DARKGRAY);

            BeginMode3D(camera);

                DrawModel(sphereModel, Vector3{20.0f, 20.0f, 0.0f}, 1.0f, WHITE);
                DrawModelWires(sphereModel, Vector3{20.0f, 20.0f, 0.0f}, 1.0f, BLACK);
                DrawModel(floorModel, Vector3{0.0f, 0.0f, 0.0f}, 1.0f, RED);

                rlPushMatrix();
                    
                    //DrawCapsule();

                rlPopMatrix();

            EndMode3D();

            static Vector3 cameraUp;
            static Vector3 cameraRight;
            static Vector3 cameraForward;

            cameraUp = GetCameraUp();
            cameraRight = GetCameraRight();
            cameraForward = GetCameraForward();

            DrawText(TextFormat("Mouse Delta (%f, %f)", mousePositionDelta.x, mousePositionDelta.y), 5.0f, 20.0f, 24.0f, WHITE);
            DrawText(TextFormat("Camera Position (%f, %f, %f)", camera.position.x, camera.position.y, camera.position.z), 5.0f, 50.0f, 24.0f, WHITE);
            DrawText(TextFormat("Camera Forward (%f, %f, %f)", cameraForward.x, cameraForward.y, cameraForward.z), 5.0f, 80.0f, 24.0f, WHITE);
            DrawText(TextFormat("Camera Right (%f, %f, %f)", cameraRight.x, cameraRight.y, cameraRight.z), 5.0f, 110.0f, 24.0f, WHITE);
            DrawText(TextFormat("Camera Up (%f, %f, %f)", cameraUp.x, cameraUp.y, cameraUp.z), 5.0f, 140.0f, 24.0f, WHITE);
            DrawText(TextFormat("Yaw (%f)", yaw), 5.0f, 200.0f, 24.0f, WHITE);
            DrawText(TextFormat("Pitch (%f)", pitch), 5.0f, 230.0f, 24.0f, WHITE);

            Ray ray;
            ray.position = {0.0f, 0.0f, 0.0f};
            ray.direction = {0.0f, 1000.0f, 0.0f};

            DrawRay(ray, GREEN);
        EndDrawing();
    }

    NetDeinit();
    CloseWindow();
    
    return 0;
}

void UpdateCamera()
{
    mousePositionDelta = GetMouseDelta();

    yaw += mousePositionDelta.x*mouseSensity*GetFrameTime();
    pitch -= mousePositionDelta.y*mouseSensity*GetFrameTime();


    if (pitch > 89.0f*DEG2RAD) pitch = 89.0f*DEG2RAD;
    else if (pitch < -89.0f*DEG2RAD) pitch = -89.0f*DEG2RAD;


    directionVector = VECTOR_CAST(Vector3)GetCameraForward();


    camera.target = Vector3Add(camera.position, directionVector);
}

void HandlePlayerMovement()
{
    #define MOVESPEED (movementSpeed * GetFrameTime())

    Vector3 right = Vector3Scale(GetCameraRight(), MOVESPEED);
    Vector3 forward = GetCameraForward();
    forward.y = 0.0f;   
    forward = Vector3Scale(Vector3Normalize(forward), MOVESPEED);

    if (IsKeyDown(KEY_W))
    {
        camera.position = Vector3Add(camera.position, forward);
        //playerBody->setTransform(rp::Transform(playerBody->getTransform().getPosition() + forward, rp::Quaternion::identity()));
    }
    if (IsKeyDown(KEY_S))
    {
        camera.position = Vector3Add(camera.position, Vector3Negate(forward));
        //playerBody->setTransform(rp::Transform(playerBody->getTransform().getPosition() - forward, rp::Quaternion::identity()));
    }
    if (IsKeyDown(KEY_A))
    {
        camera.position = Vector3Add(camera.position, Vector3Negate(right));
        //playerBody->setTransform(rp::Transform(playerBody->getTransform().getPosition() - right, rp::Quaternion::identity()));
    }
    if (IsKeyDown(KEY_D))
    {
        camera.position = Vector3Add(camera.position, right);
        //playerBody->setTransform(rp::Transform(playerBody->getTransform().getPosition() + right, rp::Quaternion::identity()));
    }
    if (IsKeyDown(KEY_SPACE))
    {
        camera.position.y += MOVESPEED;
        //playerBody->applyLocalForceAtCenterOfMass({0.0f, 20.0f, 0.0f});
    } 
    if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        camera.position.y -= MOVESPEED;
    }
}

void ConnectToServer()
{

}

void InitPlayerEntity()
{
    // Init player entity
    playerEntity = worldScene.create();

    worldScene.emplace<Vector3>(playerEntity, camera.position);
}

void UpdatePlayerLocation()
{
    worldScene.replace<Vector3>(playerEntity, camera.position);
}