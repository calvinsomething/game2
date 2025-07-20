#include "Cube.h"

Cube::Cube(Gfx &gfx) : gfx(gfx)
{
    transform = DirectX::XMMatrixIdentity();
}

DirectX::XMMATRIX Cube::get_transform()
{
    return transform * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

void Cube::bind()
{
}

void Cube::update(const DirectX::XMMATRIX &xform)
{
    transform *= xform;
}

void Cube::set_position(float x, float y, float z)
{
    position = DirectX::XMFLOAT3(x, y, z);
}

void Cube::translate(float x, float y, float z)
{
    position.x += x;
    position.y += y;
    position.z += z;
}
