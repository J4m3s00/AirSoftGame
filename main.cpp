#include "reactphysics3d/reactphysics3d.h"
#include <iostream>

#include "raylib.h"
#include "raymath.h"

namespace rp = reactphysics3d;

float mouseSensity = 1.25f;
float yaw = 0.0f;
float pitch = 0.0f;

Vector2 mousePositionDelta = {0.0f, 0.0f};
Vector3 directionVector = {0.0f, 0.0f, 0.0f};
Camera3D camera = {0};

void UpdateCamera();

int main()
{
    rp::PhysicsCommon physicsCommon;

    rp::PhysicsWorld::WorldSettings settings; 
    settings.defaultVelocitySolverNbIterations = 20; 
    settings.isSleepingEnabled = false; 
    settings.gravity = rp::Vector3(0, -9.81, 0); 


    rp::PhysicsWorld* world = physicsCommon.createPhysicsWorld(settings);

    rp::Vector3 position(0, 40, 0);
    rp::Quaternion orientation = rp::Quaternion::identity();
    rp::Transform transform(position, orientation);

    float radius = 10.0f;
    rp::RigidBody* sphereBody = world->createRigidBody(transform);
    rp::SphereShape* sphereShape = physicsCommon.createSphereShape(radius);
    rp::Collider* sphereCollider = sphereBody->addCollider(sphereShape, rp::Transform::identity());

    Vector3 floorSize = {10000.0f, 5.0f, 10000.0f};
    rp::RigidBody* floorBody = world->createRigidBody(rp::Transform::identity());
    floorBody->setType(rp::BodyType::STATIC);
    rp::BoxShape* floorShape = physicsCommon.createBoxShape((*(rp::Vector3*)&floorSize)/2.0f);
    rp::Collider* floorCollider = floorBody->addCollider(floorShape, rp::Transform::identity());

    // PLAYER BODY
    const float playerHeight = 200;
    const float playerRadius = 20;
    const rp::Transform playerStartTransform({0, 40, 0}, rp::Quaternion::identity());

    rp::RigidBody* playerBody = world->createRigidBody(playerStartTransform);
    rp::CapsuleShape* playerShape = physicsCommon.createCapsuleShape(playerRadius, playerHeight);
    rp::Collider* playerCollider = playerBody->addCollider(playerShape, rp::Transform({0, (playerHeight - 20) / 2.0f, 0}, rp::Quaternion::identity()));
    

    const float timeStep = 1 / 60.0f;

    InitWindow(1270, 720, "Air Soft");

    
    camera.position = { 20.0f, 50.0f, 30.0f };
    camera.target = { 30.0f, 50.0f, 30.0f };
    camera.up = { 0.0f, 1.0f, 0.0f };
    camera.fovy = 60.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    SetCameraMode(camera, CAMERA_CUSTOM);
    SetTargetFPS(60);
    DisableCursor();

    while (!WindowShouldClose())
    {
        //rp::Matrix3x3 currentPlayerRotation = playerBody->getTransform().getOrientation().getMatrix();

        UpdateCamera();
        world->update(timeStep);

        camera.position = *(Vector3*)&playerBody->getTransform().getPosition();
        

        if (IsKeyPressed(KEY_SPACE))
        {
            playerBody->applyLocalForceAtCenterOfMass({0.0f, 2000.0f, 0.0f});
        }  


        BeginDrawing(); 
            ClearBackground(DARKGRAY);

            BeginMode3D(camera);

                DrawSphere(*(Vector3*)&sphereBody->getTransform().getPosition(), radius, WHITE);
                DrawCubeV(*(Vector3*)&(floorBody->getTransform().getPosition()), floorSize, RED);

            EndMode3D();

            DrawText(TextFormat("Mouse Delta (%f, %f)", mousePositionDelta.x, mousePositionDelta.y), 5.0f, 20.0f, 24.0f, WHITE);
            DrawText(TextFormat("Camera Position (%f, %f, %f)", camera.position.x, camera.position.y, camera.position.z), 5.0f, 50.0f, 24.0f, WHITE);
            DrawText(TextFormat("Camera Target (%f, %f, %f)", camera.target.x, camera.target.y, camera.target.z), 5.0f, 80.0f, 24.0f, WHITE);
            DrawText(TextFormat("Direction Vector (%f, %f, %f)", directionVector.x, directionVector.y, directionVector.z), 5.0f, 110.0f, 24.0f, WHITE);
            DrawText(TextFormat("Yaw (%f)", yaw), 5.0f, 140.0f, 24.0f, WHITE);
            DrawText(TextFormat("Pitch (%f)", pitch), 5.0f, 170.0f, 24.0f, WHITE);

            Ray ray;
            ray.position = {0.0f, 0.0f, 0.0f};
            ray.direction = {0.0f, 1000.0f, 0.0f};

            DrawRay(ray, GREEN);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

void UpdateCamera()
{
    mousePositionDelta = GetMouseDelta();

    yaw += mousePositionDelta.x*mouseSensity*GetFrameTime();
    pitch += mousePositionDelta.y*mouseSensity*GetFrameTime();


    if (pitch > 89.0f) pitch = 89.0f;
    else if (pitch < -89.0f) pitch = -89.0f;

    float forwardX = cosf(yaw) * cos(pitch);
    float forwardY = sin(pitch);
    float forwardZ = sin(yaw) * cos(pitch);

    directionVector = {forwardX, forwardY, forwardZ};
    
    Vector3 newCameraTarget = {camera.position.x + forwardX, camera.position.y + forwardY, camera.position.z + forwardZ};

    printf("NewCameraTarget: (%f, %f, %f)\n", newCameraTarget.x, newCameraTarget.y, newCameraTarget.z);

    camera.target = newCameraTarget;
}