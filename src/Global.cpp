#include "Global.h"

namespace Global
{
bool running = true;

DirectX::XMFLOAT3 position{};
DirectX::XMFLOAT3 twelve_oclock{0.0f, 0.0f, 1.0f};
DirectX::XMFLOAT3 nine_oclock{-1.0f, 0.0f, 0.0f};

Clock clock;

LightingData lighting_data{
    DirectX::XMFLOAT4{50.0f, 100.0f, 0.0f, 0.3f},
};

}; // namespace Global
