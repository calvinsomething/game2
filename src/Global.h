#pragma once

#include <DirectXMath.h>

#include "game/Clock.h"
#include "game/CollisionSystem.h"
#include "input/Input.h"

namespace Global
{
extern bool running;

extern int client_width, client_height;
extern float client_aspect_ratio;

extern DirectX::XMVECTOR north;

extern float movement_speed;

extern Input input;

extern Clock clock;

extern CollisionSystem collision_system;

struct alignas(16) LightingData
{
    DirectX::XMFLOAT4 light_position_and_ambient_amount;
    DirectX::XMMATRIX light_view_proj;
};

extern LightingData lighting_data;

}; // namespace Global
