#include "Cube.h"

Cube::Cube(Gfx &gfx) : gfx(gfx)
{
    transforms.model = DirectX::XMMatrixIdentity();
    transforms.world = DirectX::XMMatrixIdentity();
}

Cube::Transforms *Cube::get_transforms()
{
    return &transforms;
}

void Cube::bind()
{
}

void Cube::update()
{
    transforms.model *= DirectX::XMMatrixRotationRollPitchYaw(0.005f, 0.002f, 0.003f);

    // vs->set_buffer(&buffer_data);
}
