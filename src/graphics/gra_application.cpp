#include "air_graphics.h"



#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"


using namespace AirSoft;

Application::Application()
{
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1270, 720, "Air Soft");

    fPlayerCamera.position = { 0.0f, 50.0f, 0.0f };
    fPlayerCamera.target = { 30.0f, 50.0f, 30.0f };
    fPlayerCamera.up = { 0.0f, 1.0f, 0.0f };
    fPlayerCamera.fovy = 60.0f;
    fPlayerCamera.projection = CAMERA_PERSPECTIVE;

    
    SetCameraMode(fPlayerCamera, CAMERA_CUSTOM);
    SetTargetFPS(60);
    DisableCursor();


// Load default world shader
    fWorldShader = LoadShaderFromMemory(
#include "lighting_vs.h"
,
#include "lighting_fs.h"
    );

    fWorldShader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(fWorldShader, "viewPos");


    int ambientLoc = GetShaderLocation(fWorldShader, "ambient");
    Vector4 ambient = {0.1f, 0.1f, 0.1f, 0.1f};
    SetShaderValue(fWorldShader, ambientLoc, &ambient.x, SHADER_UNIFORM_VEC4);


    fLights[MAX_LIGHTS] = { 0 };
    fLights[0] = CreateLight(LIGHT_DIRECTIONAL, Vector3{ -2000, 1000, -2000 }, Vector3Zero(), WHITE, fWorldShader);




    // MODELS
    Vector3 floorSize = {100.0f, 1.0f, 100.0f};
    float radius = 10.0f;

    
    fSphereModel = LoadModelFromMesh(GenMeshSphere(radius, 32, 32));
    fSphereModel.materials[0].shader = fWorldShader;


    fFloorModel = LoadModelFromMesh(GenMeshCube(floorSize.x, floorSize.y, floorSize.z));
    fFloorModel.materials[0].shader = fWorldShader;

}

Application::~Application()
{
    CloseWindow();
}

void Application::Update()
{
    float cameraPos[3] = { fPlayerCamera.position.x, fPlayerCamera.position.y, fPlayerCamera.position.z };
    SetShaderValue(fWorldShader, fWorldShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);


    
    for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(fWorldShader, fLights[i]);


    BeginDrawing(); 
        ClearBackground(DARKGRAY);

        BeginMode3D(fPlayerCamera);

            DrawModel(fSphereModel, Vector3{20.0f, 20.0f, 0.0f}, 1.0f, WHITE);
            DrawModelWires(fSphereModel, Vector3{20.0f, 20.0f, 0.0f}, 1.0f, BLACK);
            DrawModel(fFloorModel, Vector3{0.0f, 0.0f, 0.0f}, 1.0f, RED);

            rlPushMatrix();
                
                //DrawCapsule();

            rlPopMatrix();

        EndMode3D();


        DrawText(TextFormat("Mouse Delta ()"), 5.0f, 20.0f, 24.0f, WHITE);
    EndDrawing();
}

Camera3D& Application::GetPlayerCamera()
{
    return fPlayerCamera;
}

const Camera3D& Application::GetPlayerCamera() const
{
    return fPlayerCamera;
}