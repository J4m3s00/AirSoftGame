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
        std::unordered_map<uint32_t, entt::entity> fOnlinePlayerMap;
    public:
        Scene();
        ~Scene();

        entt::entity AddOnlinePlayer(uint32_t playerId);
        entt::entity GetOnlinePlayer(uint32_t playerId);
        void RemoveOnlinePlayer(uint32_t playerid);

        entt::registry& GetRegistry();
        const entt::registry& GetRegistry() const;
    };

}