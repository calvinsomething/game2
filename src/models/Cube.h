#pragma once

#include "../Gfx/VertexShader.h"

class Cube
{
  public:
    Cube(Gfx &gfx);

    void bind();

    void update(const DirectX::XMMATRIX &xform);

    void set_position(float x, float y, float z);
    void translate(float x, float y, float z);

    DirectX::XMMATRIX get_transform();

  private:
    Gfx &gfx;

    VertexShader::BufferData buffer_data;

    DirectX::XMMATRIX transform;

    DirectX::XMFLOAT3 position;
};
