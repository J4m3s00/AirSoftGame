#include "air_graphics.h"

#include "external/par_shapes.h"



#define RLIGHTS_IMPLEMENTATION
#include "rlights.h"

#define SCREEN_WIDTH 1270
#define SCREEN_HEIGHT 720

#define SCREEN_CENTER_X (SCREEN_WIDTH / 2)
#define SCREEN_CENTER_Y (SCREEN_HEIGHT / 2)


using namespace AirSoft;


static PlayerControllSettings settings = {
    /*.MoveSpeed = */50.0f,
    /*.Sensity = */1.2f
}; // Load from file in future

static CrosshairSettings crosshairSettings = {
    3, /*.Gab*/
    5, /*.Size*/
    3, /*.Thickness*/
    0.5, /*.OutlineThickness*/
    true, /*.ThrawOutline*/
    true /*.Dot*/
};

void ChangeCrosshairByKey()
{
    /*
     *  r = Gap+  |  t = Size+  |  z = thick+  |  u = outline+  |  i = outline?
     *  f = Gab-  |  g = Size-  |  h = thick-  |  j = outline-  |  k = dot?
     * 
     *  1 = normal steps
     *  0.5 = small steps (hold lshift)
     */
    
    float steps = IsKeyDown(KEY_LEFT_SHIFT) ? 0.5f : 1.0f;
    
    if (IsKeyPressed(KEY_R)) crosshairSettings.Gab += steps;
    if (IsKeyPressed(KEY_F)) crosshairSettings.Gab -= steps;
    if (IsKeyPressed(KEY_T)) crosshairSettings.Size += steps;
    if (IsKeyPressed(KEY_G)) crosshairSettings.Size -= steps;
    if (IsKeyPressed(KEY_Y)) crosshairSettings.Thickness += steps;
    if (IsKeyPressed(KEY_H)) crosshairSettings.Thickness -= steps;
    if (IsKeyPressed(KEY_U)) crosshairSettings.OutlineThickness += steps;
    if (IsKeyPressed(KEY_J)) crosshairSettings.OutlineThickness -= steps;
    if (IsKeyPressed(KEY_I)) crosshairSettings.DrawOutline = !crosshairSettings.DrawOutline;
    if (IsKeyPressed(KEY_K)) crosshairSettings.Dot = !crosshairSettings.Dot;
}

Application::Application(NetClient* client)
    : fClient(client), fPlayerController(Scene::GetCurrentRegistry().create(), settings)
{
    assert(client);
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Air Soft");
    SetExitKey(0);

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

    fCursorEnabled = false;
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
    float radius = GameInstance::Instance().GetPlayerDimensions().Radius;
    float height = GameInstance::Instance().GetPlayerDimensions().Height;


      
    fSphereModel = LoadModelFromMesh(GenMeshCylinder(radius, height, 16));
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
    if (/*IsKeyPressed(KEY_PERIOD) || */IsKeyPressed(KEY_ESCAPE))
    {
        fCursorEnabled = !fCursorEnabled;
        //fConsole.ToggleVisibility();
    }
    fCursorEnabled ? EnableCursor() : DisableCursor();
    if (!fCursorEnabled)
    {
        HandlePlayerMovement();
    }

    ChangeCrosshairByKey();

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
                auto dimensions = GameInstance::Instance().GetPlayerDimensions();
                DrawCapsuleWires(VECTOR_CAST(Vector3)opc.Position, {opc.Position.x, opc.Position.y - dimensions.Height, opc.Position.z}, dimensions.Radius, 48, 48, WHITE);
                //DrawModel(fSphereModel, VECTOR_CAST(Vector3)opc.Position, 1.0f, WHITE);                
            }

            //DrawModel(fSphereModel, Vector3{20.0f, 20.0f, 0.0f}, 1.0f, WHITE);
            //DrawModelWires(fSphereModel, Vector3{20.0f, 20.0f, 0.0f}, 1.0f, BLACK);
            DrawModel(fFloorModel, Vector3{0.0f, -5.0f, 0.0f}, 1.0f, RED);

            Ray r{};
            r.position = {};
            r.direction = {1.0f, 0.0f, 0.0f};
            DrawRay(r, BLUE);
            r.direction = {0.0f, 1.0f, 0.0f};
            DrawRay(r, RED);
            r.direction = {0.0f, 0.0f, 1.0f};
            DrawRay(r, GREEN);

            rlPushMatrix();
                
                //DrawCapsule();

            rlPopMatrix();


        EndMode3D();

        auto coreView = registry.view<PlayerScoreComponent>();
        int i = 0;
        for (auto entity : coreView)
        {
            uint32_t score = registry.get<PlayerScoreComponent>(entity).Points;
            DrawText(TextFormat("Score: %u", score), 5.0f, 20.0f + (i++ * 20.0f), 24.0f, WHITE);
        }

        // Draw crosshair last to overlay everything
        DrawCrossHair();

        fConsole.Render();
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
    input.Crouch = IsKeyDown(KEY_LEFT_CONTROL);
    input.MouseDelta = VECTOR_CAST(Vec2)GetMouseDelta();

    PlayerComponent& player = Scene::GetCurrentRegistry().get<PlayerComponent>(fPlayerController.GetPlayerEntity());
    if (fPlayerController.ApplyInput(input, GetFrameTime()))
    {
        fClient->Send_PlayerMove(player.Position, {});
    }

    if (IsMouseButtonDown(0))
    {
        PPlayer_Shoot ps;
        ps.Direction = GetCameraForward(player.Yaw * DEG2RAD, player.Pitch * DEG2RAD);
        ps.Position = player.Position;
        fClient->Send_PlayerShoot(ps);
    }
}

void Application::DrawCrossHair()
{
    /**         _
     *         | |  
     *         |_|     
     *  _____       _____
     * |_____| gab |_____| <- thickness
     *   ^      _  
     *   |     | | <- height
     *  width  |_| 
     */

    Rectangle rLeft;
    Rectangle rRight;
    Rectangle rTop;
    Rectangle rBottom;

    // Left
    rLeft.x = SCREEN_CENTER_X - (crosshairSettings.Gab + crosshairSettings.Size);
    rLeft.y = SCREEN_CENTER_Y - (crosshairSettings.Thickness / 2);
    rLeft.width = crosshairSettings.Size;
    rLeft.height = crosshairSettings.Thickness;

    rRight.x = SCREEN_CENTER_X + crosshairSettings.Gab;
    rRight.y = SCREEN_CENTER_Y - (crosshairSettings.Thickness / 2);
    rRight.width = crosshairSettings.Size;
    rRight.height = crosshairSettings.Thickness;

    rTop.x = SCREEN_CENTER_X - crosshairSettings.Thickness / 2;
    rTop.y = SCREEN_CENTER_Y - (crosshairSettings.Gab + crosshairSettings.Size);
    rTop.width = crosshairSettings.Thickness;
    rTop.height = crosshairSettings.Size;

    rBottom.x = SCREEN_CENTER_X - crosshairSettings.Thickness / 2;
    rBottom.y = SCREEN_CENTER_Y + crosshairSettings.Gab;
    rBottom.width = crosshairSettings.Thickness;
    rBottom.height = crosshairSettings.Size;


    DrawRectangleRec(rLeft, GREEN);
    DrawRectangleRec(rRight, GREEN);
    DrawRectangleRec(rTop, GREEN);
    DrawRectangleRec(rBottom, GREEN);

    if (crosshairSettings.DrawOutline)
    {
        DrawRectangleLinesEx(rLeft, crosshairSettings.OutlineThickness, BLACK);
        DrawRectangleLinesEx(rRight, crosshairSettings.OutlineThickness, BLACK);
        DrawRectangleLinesEx(rTop, crosshairSettings.OutlineThickness, BLACK);
        DrawRectangleLinesEx(rBottom, crosshairSettings.OutlineThickness, BLACK);
    }
    if (crosshairSettings.Dot)
    {
        #define DOT_RAD 2

        DrawCircle(SCREEN_CENTER_X, SCREEN_CENTER_Y, DOT_RAD, GREEN);
        if (crosshairSettings.DrawOutline)
        {
            DrawCircleLines(SCREEN_CENTER_X, SCREEN_CENTER_Y, DOT_RAD, BLACK);
        }
    }   
}