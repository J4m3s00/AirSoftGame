#pragma once

#define MAX_LIGHTS 4

namespace AirSoft {

    struct CrosshairSettings
    {
        float Gab;
        float Size;
        float Thickness;
        float OutlineThickness;

        bool DrawOutline;
        bool Dot;
    };

    class Application
    {
    private:
        // This may change
        Camera3D fPlayerCamera;
        PlayerController fPlayerController;

        Shader   fWorldShader;
        Light    fLights[MAX_LIGHTS];
        Model    fSphereModel;
        Model    fFloorModel;
        NetClient* fClient;
    public:
        Application(NetClient* client);
        ~Application();

        void Update();

        Camera3D& GetPlayerCamera();
        const Camera3D& GetPlayerCamera() const;
    private:
        void HandlePlayerMovement();
        void DrawCrossHair();
    };

}
