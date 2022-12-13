#pragma once

namespace AirSoft {

    
    struct OnlinePlayerComponent 
    {
        uint32_t PlayerID;
        Vec3     Position;
    };

    class Scene 
    {
    private:
        entt::registry fRegistry;
        rp::PhysicsCommon* fPhysicsCommon;
        rp::PhysicsWorld* fPhysicsWorld;

        std::unordered_map<uint32_t, entt::entity> fUserIdToEntityMap;
        std::unordered_map<entt::entity, uint32_t> fEntityToUserIdMap;
        std::unordered_map<entt::entity, rp::CollisionBody*> fEntityPhysicsObjectMap;
    public:
        Scene(rp::PhysicsCommon* common);
        ~Scene();

        entt::entity AddOnlinePlayer(uint32_t playerId);
        entt::entity GetOnlinePlayer(uint32_t playerId);
        void RemoveOnlinePlayer(uint32_t playerid);
        uint32_t GetUserId(entt::entity entity);

        rp::CollisionBody* GetEntityCollisionBody(entt::entity entity);

        entt::registry& GetRegistry();
        const entt::registry& GetRegistry() const;

        rp::PhysicsCommon* GetPhysicsCommon() const;
        rp::PhysicsWorld* GetPhysicsWorld() const;

        static Scene* GetCurrentScene();

        static entt::registry& GetCurrentRegistry();
    };

}