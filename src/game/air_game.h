#pragma once
#include "reactphysics3d/reactphysics3d.h"

#include "entt/entt.hpp"

namespace rp = reactphysics3d;

using Vec2 = rp::Vector2;
using Vec3 = rp::Vector3;
using Mat3 = rp::Matrix3x3;

#define VECTOR_CAST(type) *(type*)&

#include "game_scene.h"