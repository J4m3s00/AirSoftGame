#pragma once

namespace AirSoft {

    struct PlayerScoreComponent
    {
        uint32_t Points;
    };

    // Component
    struct PlayerComponent 
    {
        Vec3 Position;
        float Yaw; // In Degrees
        float Pitch; // In Degrees
    };


    // Controller
    struct PlayerControllSettings
    {
        float MoveSpeed;
        float Sensity;
    };

    struct PlayerInput
    {
        bool Forward = false;
        bool Backward = false;
        bool Right = false;
        bool Left = false;

        bool Jump = false;
        bool Crouch = false;

        bool Shoot = false;

        Vec2 MouseDelta = {};
    };
    
    class PlayerController 
    {
    private:
        entt::entity           fPlayerEntity;
        PlayerControllSettings fSettings;
    public:
        PlayerController(entt::entity playerEntity, const PlayerControllSettings& settings);
        ~PlayerController() = default;

        /**
         * @brief Applys user input to the entity
         * 
         * @param input 
         * @param frameTime 
         * @return true Update client
         * @return false Dont update client
         */
        bool ApplyInput(const PlayerInput& input, float frameTime);


        entt::entity GetPlayerEntity() const;
    };

}