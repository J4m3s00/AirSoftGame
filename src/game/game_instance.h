#pragma once

namespace AirSoft {

    // Load from file
    struct Settings_PlayerDimensions
    {
        float Radius = 5.0f;
        float Height = 20.0f;
    };

    class GameInstance
    {
    private:
        Scene* fCurrentScene;
        std::vector<Scene*> fLoadedScenes;

        rp::PhysicsCommon fPhysicsCommon;

        // Settings
        Settings_PlayerDimensions fPlayerDimensions;
    public:
        GameInstance();
        ~GameInstance();

        Scene* CreateScene();
        void SetCurrentScene(Scene* scene);
        Scene* GetCurrentScene();

        const Settings_PlayerDimensions& GetPlayerDimensions() const;

        static GameInstance& Instance();
    };

}