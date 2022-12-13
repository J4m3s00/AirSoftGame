#include "air_graphics.h"



#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"


using namespace AirSoft;


static PlayerControllSettings settings = {
    /*.MoveSpeed = */50.0f,
    /*.Sensity = */1.2f
}; // Load from file in future

Application::Application(NetClient* client)
    : fClient(client), fPlayerController(Scene::GetCurrentRegistry().create(), settings)
{
    assert(client);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1270, 720, "Air Soft");

    //Set Player position and camera
    const Vec3 START_POS = { 0.0f, 50.0f, 0.0f };
    Scene::GetCurrentRegistry().get<PlayerComponent>(fPlayerController.GetPlayerEntity()).Position = START_POS;

    fPlayerCamera.position = VECTOR_CAST(Vector3)START_POS;
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
    float radius = 2.0f;

    
    fSphereModel = LoadModelFromMesh(GenMeshKnot(radius, 5.0f, 64, 64));
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
    HandlePlayerMovement();
    entt::registry& registry = Scene::GetCurrentRegistry();
    PlayerComponent& player = registry.get<PlayerComponent>(fPlayerController.GetPlayerEntity());

    // Update Camera to player
    Vec3 target = player.Position + GetCameraForward(player.Yaw*DEG2RAD, player.Pitch*DEG2RAD);

    fPlayerCamera.position = VECTOR_CAST(Vector3)player.Position;
    fPlayerCamera.target = VECTOR_CAST(Vector3)target;
    

    float cameraPos[3] = { fPlayerCamera.position.x, fPlayerCamera.position.y, fPlayerCamera.position.z };
    SetShaderValue(fWorldShader, fWorldShader.locs[SHADER_LOC_VECTOR_VIEW], cameraPos, SHADER_UNIFORM_VEC3);
    for (int i = 0; i < MAX_LIGHTS; i++) UpdateLightValues(fWorldShader, fLights[i]);

    BeginDrawing(); 
        ClearBackground(DARKGRAY);

        // Handle player input

        BeginMode3D(fPlayerCamera);

            auto view = registry.view<OnlinePlayerComponent>();
            for (auto entry : view)
            {
                if (!registry.valid(entry)) { continue; }
                //printf("Rendering player\n");
                OnlinePlayerComponent opc = registry.get<OnlinePlayerComponent>(entry);
                DrawModel(fSphereModel, VECTOR_CAST(Vector3)opc.Position, 1.0f, WHITE);                
            }

            //DrawModel(fSphereModel, Vector3{20.0f, 20.0f, 0.0f}, 1.0f, WHITE);
            //DrawModelWires(fSphereModel, Vector3{20.0f, 20.0f, 0.0f}, 1.0f, BLACK);
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

void Application::HandlePlayerMovement()
{
    PlayerInput input;
    input.Forward = IsKeyDown(KEY_W); // Have keybindings
    input.Backward = IsKeyDown(KEY_S);
    input.Right = IsKeyDown(KEY_D);
    input.Left = IsKeyDown(KEY_A);
    input.Jump = IsKeyDown(KEY_SPACE);
    input.MouseDelta = VECTOR_CAST(Vec2)GetMouseDelta();

    if (fPlayerController.ApplyInput(input, GetFrameTime()))
    {
        PlayerComponent& player = Scene::GetCurrentRegistry().get<PlayerComponent>(fPlayerController.GetPlayerEntity());

        fClient->Send_PlayerMove(player.Position, {});
    }
}