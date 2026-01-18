#include "Global.h"

namespace Global
{
bool running = true;

DirectX::XMVECTOR north{0.0f, 0.0f, 1.0f, 0.0f};

float movement_speed = 0;

Input input;

Clock clock;

LightingData lighting_data{
    DirectX::XMFLOAT4{50.0f, 100.0f, 0.0f, // diffuse
                      0.3f},               // ambient
};

}; // namespace Global
