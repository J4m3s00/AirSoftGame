#include "air_game.h"

using namespace AirSoft;

Scene::Scene()
{
    
}

Scene::~Scene()
{
    
}

entt::entity Scene::AddOnlinePlayer(uint32_t playerId)
{
    entt::entity result = fRegistry.create();
    fRegistry.emplace<OnlinePlayerComponent>(result, OnlinePlayerComponent{playerId});

    fOnlinePlayerMap.insert({playerId, result});

    return result;
}

entt::entity Scene::GetOnlinePlayer(uint32_t playerId)
{
    if (fOnlinePlayerMap.find(playerId) == fOnlinePlayerMap.end())
    {
        return entt::null;
    }
    return fOnlinePlayerMap.at(playerId);
}

void Scene::RemoveOnlinePlayer(uint32_t playerid)
{
    if (fOnlinePlayerMap.find(playerid) != fOnlinePlayerMap.end())
    {
        fRegistry.destroy(fOnlinePlayerMap.at(playerid));
        fOnlinePlayerMap.erase(playerid);
    }
}

entt::registry& Scene::GetRegistry()
{
    return fRegistry;
}

const entt::registry& Scene::GetRegistry() const
{
    return fRegistry;
}
