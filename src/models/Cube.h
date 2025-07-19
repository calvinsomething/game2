#pragma once

#include "../Gfx/VertexShader.h"

class Cube
{
  public:
    struct Transforms
    {
        DirectX::XMMATRIX model;
        DirectX::XMMATRIX world;
    };

    Cube(Gfx &gfx);

    void bind();

    void update();

    Transforms *get_transforms();

  private:
    Gfx &gfx;

    bool initialized = false;

    VertexShader::BufferData buffer_data;

    Transforms transforms;
};
