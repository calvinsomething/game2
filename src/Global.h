#pragma once

#include <DirectXMath.h>

#include "game/Clock.h"

namespace Global
{
extern bool running;

extern DirectX::XMFLOAT3 position;
extern DirectX::XMFLOAT3 position_delta;
extern DirectX::XMFLOAT3 twelve_oclock;
extern DirectX::XMFLOAT3 three_oclock;

extern Clock clock;

struct alignas(16) LightingData
{
    DirectX::XMFLOAT4 light_position_and_ambient_amount;
};

extern LightingData lighting_data;

}; // namespace Global
