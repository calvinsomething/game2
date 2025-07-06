#pragma once

#include "ConstantBuffer.h"

class Camera
{
  public:
    struct BufferData
    {
        DirectX::XMMATRIX view_proj_xform;
    };

    Camera(Gfx &gfx);

    void update();

  private:
    ConstantBuffer constant_buffer;

    BufferData buffer_data;
};
