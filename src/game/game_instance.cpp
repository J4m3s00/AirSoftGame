#include "air_game.h"

using namespace AirSoft;

GameInstance::GameInstance()
    : fCurrentScene(NULL)
{
}

GameInstance::~GameInstance()
{
    for (Scene* scene : fLoadedScenes)
    {
        delete scene;
    }   
    fLoadedScenes.clear();
    fCurrentScene = nullptr;
}

Scene* GameInstance::CreateScene()
{
    Scene* result = new Scene(&fPhysicsCommon);
    fLoadedScenes.push_back(result);
    if (!fCurrentScene)
    {
        fCurrentScene = result;
    }
    return result;
}

void GameInstance::SetCurrentScene(Scene* scene)
{
    fCurrentScene = scene;
}

Scene* GameInstance::GetCurrentScene()
{
    return fCurrentScene;
}

const Settings_PlayerDimensions& GameInstance::GetPlayerDimensions() const
{
    return fPlayerDimensions;
}

GameInstance& GameInstance::Instance()
{
    static GameInstance instance;
    return instance;
}