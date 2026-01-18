#pragma once

#include <DirectXMath.h>

#include "game/Clock.h"
#include "input/Input.h"

namespace Global
{
extern bool running;

extern DirectX::XMVECTOR north;

extern float movement_speed;

extern Input input;

extern Clock clock;

struct alignas(16) LightingData
{
    DirectX::XMFLOAT4 light_position_and_ambient_amount;
};

extern LightingData lighting_data;

}; // namespace Global
