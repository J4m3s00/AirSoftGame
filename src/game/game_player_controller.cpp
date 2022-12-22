#include "air_game.h"

using namespace AirSoft;

PlayerController::PlayerController(entt::entity playerEntity, const PlayerControllSettings& settings)
    : fPlayerEntity(playerEntity), fSettings(settings)
{
    if (Scene::GetCurrentScene())
    {
        entt::registry& registry = Scene::GetCurrentScene()->GetRegistry();
        if (!registry.any_of<PlayerComponent>(playerEntity))
        {
            registry.emplace<PlayerComponent>(playerEntity);
        }
    }
}

bool PlayerController::ApplyInput(const PlayerInput& input, float frameTime)
{
    bool changed = input.Backward || input.Forward || input.Left || input.Right || input.Jump || input.Crouch || input.MouseDelta.lengthSquare() != 0;

    // Direction looking from above
    Vec3 moveDirection = {};
    PlayerComponent& component = Scene::GetCurrentRegistry().get<PlayerComponent>(fPlayerEntity);

    // Apply mouse first
    component.Yaw += input.MouseDelta.x * fSettings.Sensity * frameTime;
    component.Pitch -= input.MouseDelta.y * fSettings.Sensity * frameTime;

    if (component.Pitch > 89.0f) component.Pitch = 89.0f;
    else if (component.Pitch < -89.0f) component.Pitch = -89.0f;

    #define MOVE_MULT (fSettings.MoveSpeed * frameTime)

    Vec3 right = GetCameraRight(component.Yaw * DEG2RAD) * MOVE_MULT;
    Vec3 forward = GetCameraForward(component.Yaw * DEG2RAD, component.Pitch * DEG2RAD);
    forward.y = 0.0f;
    forward.normalize();

    if (input.Forward)
    {
        moveDirection = forward;
    }
    if (input.Backward)
    {
        moveDirection += -forward;
    }
    if (input.Right)
    {
        moveDirection += right;
    }
    if (input.Left)
    {
        moveDirection += -right;
    }
    if (input.Jump)
    {
        moveDirection += {0.0f, MOVE_MULT, 0.0f};
    }
    if (input.Crouch)
    {
        moveDirection += {0.0f, -MOVE_MULT, 0.0f};
    }

    component.Position += moveDirection;

    return changed;
}

entt::entity PlayerController::GetPlayerEntity() const
{
    if (Scene::GetCurrentRegistry().valid(fPlayerEntity))
    {
        return fPlayerEntity;
    }
    return entt::null;
}
