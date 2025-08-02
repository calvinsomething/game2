#include "util.h"

#include <DirectXMath.h>

DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
    return DirectX::XMFLOAT3{a.x + b.x, a.y + b.y, a.z + b.z};
}

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
    return DirectX::XMFLOAT3{a.x - b.x, a.y - b.y, a.z - b.z};
}

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a)
{
    return DirectX::XMFLOAT3{a.x, a.y, a.z};
}

DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 v, float f)
{
    return DirectX::XMFLOAT3{v.x * f, v.y * f, v.z * f};
}
