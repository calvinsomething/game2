#include "Global.h"

namespace Global
{
bool running = true;

int client_width = 0, client_height = 0;
float client_aspect_ratio = 1.0f;

DirectX::XMVECTOR north{0.0f, 0.0f, 1.0f, 0.0f};

float movement_speed = 0;

Input input;

Clock clock;

CollisionSystem collision_system;

LightingData lighting_data{
    DirectX::XMFLOAT4{0.0f, 20.0f, 100.0f, // diffuse
                      0.15f},              // ambient
    DirectX::XMMatrixMultiplyTranspose(DirectX::XMMatrixLookAtLH(DirectX::XMVECTOR{0.0f, 20.0f, 100.0f, 1.0f},
                                                                 DirectX::XMVECTOR{0.0f, 0.0f, 0.0f, 1.0f},
                                                                 DirectX::XMVECTOR{0.0f, 1.0f, 0.0f, 0.0f}),
                                       DirectX::XMMatrixOrthographicLH(50.0f, 50.0f, 80.0f, 130.0f)),
};

}; // namespace Global
