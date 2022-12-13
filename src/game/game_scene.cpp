#include "air_game.h"

using namespace AirSoft;

Scene::Scene(rp::PhysicsCommon* common)
    : fPhysicsCommon(common)
{
    fPhysicsWorld = fPhysicsCommon->createPhysicsWorld();
}

Scene::~Scene()
{
    
}

entt::entity Scene::AddOnlinePlayer(uint32_t playerId)
{
    entt::entity result = fRegistry.create();
    fRegistry.emplace<OnlinePlayerComponent>(result, OnlinePlayerComponent{playerId});
    fRegistry.emplace<PlayerScoreComponent>(result);

    float radius = GameInstance::Instance().GetPlayerDimensions().Radius;
    float height = GameInstance::Instance().GetPlayerDimensions().Height;

    rp::CapsuleShape* shape = fPhysicsCommon->createCapsuleShape(radius, height);
    rp::CollisionBody* body = fPhysicsWorld->createCollisionBody(rp::Transform::identity());
    body->addCollider(shape, rp::Transform::identity());

    fEntityPhysicsObjectMap.insert({result, body});
    fUserIdToEntityMap.insert({playerId, result});
    fEntityToUserIdMap.insert({result, playerId});

    return result;
}

entt::entity Scene::GetOnlinePlayer(uint32_t playerId)
{
    if (fUserIdToEntityMap.find(playerId) == fUserIdToEntityMap.end())
    {
        return entt::null;
    }
    return fUserIdToEntityMap.at(playerId);
}

void Scene::RemoveOnlinePlayer(uint32_t playerid)
{
    if (fUserIdToEntityMap.find(playerid) != fUserIdToEntityMap.end())
    {
        fRegistry.remove<OnlinePlayerComponent>(fUserIdToEntityMap.at(playerid));
        fRegistry.destroy(fUserIdToEntityMap.at(playerid));

        fEntityToUserIdMap.erase(fUserIdToEntityMap.at(playerid));
        fEntityPhysicsObjectMap.erase(fUserIdToEntityMap.at(playerid));


        fUserIdToEntityMap.erase(playerid);
    }
}

uint32_t Scene::GetUserId(entt::entity entity)
{
    if (fEntityToUserIdMap.find(entity) == fEntityToUserIdMap.end())
    {
        printf("Could not find user for entity %u", entity);
        return 0;
    }
    return fEntityToUserIdMap.at(entity);
}

rp::CollisionBody* Scene::GetEntityCollisionBody(entt::entity entity)
{
    if (fEntityPhysicsObjectMap.find(entity) != fEntityPhysicsObjectMap.end())
    {
        return fEntityPhysicsObjectMap.at(entity);
    }
    return nullptr;
}

entt::registry& Scene::GetRegistry()
{
    return fRegistry;
}

const entt::registry& Scene::GetRegistry() const
{
    return fRegistry;
}

rp::PhysicsCommon* Scene::GetPhysicsCommon() const
{
    return fPhysicsCommon;
}

rp::PhysicsWorld* Scene::GetPhysicsWorld() const
{
    return fPhysicsWorld;
}

Scene* Scene::GetCurrentScene()
{
    return GameInstance::Instance().GetCurrentScene();
}

entt::registry& Scene::GetCurrentRegistry()
{
    assert(Scene::GetCurrentScene());
    return Scene::GetCurrentScene()->GetRegistry();
}
