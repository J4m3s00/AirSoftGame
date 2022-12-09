#pragma once

#define MAX_LIGHTS 4

namespace AirSoft {

    class Application
    {
    private:
        Camera3D fPlayerCamera;
        Shader   fWorldShader;
        Light    fLights[MAX_LIGHTS];
        Model    fSphereModel;
        Model    fFloorModel;
    public:
        Application();
        ~Application();

        void Update();

        Camera3D& GetPlayerCamera();
        const Camera3D& GetPlayerCamera() const;
    };

}
