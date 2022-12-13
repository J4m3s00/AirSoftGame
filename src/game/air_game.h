#pragma once
#include "reactphysics3d/reactphysics3d.h"

#include "entt/entt.hpp"

namespace rp = reactphysics3d;

using Vec2 = rp::Vector2;
using Vec3 = rp::Vector3;
using Mat3 = rp::Matrix3x3;

#define VECTOR_CAST(type) *(type*)&

#ifndef PI
    #define PI 3.14159265358979323846f
#endif
#ifndef DEG2RAD
    #define DEG2RAD (PI/180.0f)
#endif
#ifndef RAD2DEG
    #define RAD2DEG (180.0f/PI)
#endif


static inline Vec3 GetCameraForward(float yawAngle, float pitchAngle)
{
    float forwardX = cosf(yawAngle) * cosf(pitchAngle);
    float forwardY = sinf(pitchAngle);
    float forwardZ = sinf(yawAngle) * cosf(pitchAngle);

    Vec3 result = Vec3(forwardX, forwardY, forwardZ);
    result.normalize();
    
    return result;
}

static inline Vec3 GetCameraRight(float yawAngle)
{
    Vec3 result;
    result.x = -sin(yawAngle);
    result.y = 0.0f;
    result.z = cos(yawAngle);

    result.normalize();
    return result;
}

static inline Vec3 GetCameraUp(float yawAngle, float pitchAngle)
{
    Vec3 result = GetCameraForward(yawAngle, pitchAngle);
    result.cross(GetCameraRight(yawAngle));
    return result;
}

#include <stdio.h>
#include <map>
#include <vector>


#include "game_player_controller.h"
#include "game_scene.h"

#include "game_instance.h"