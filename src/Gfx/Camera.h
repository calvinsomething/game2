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

    void move(DirectX::FXMVECTOR diff);

  private:
    DirectX::XMVECTOR eye_pos;
    DirectX::XMVECTOR focus_pos;
    DirectX::XMVECTOR up_dir;

    ConstantBuffer constant_buffer;

    BufferData buffer_data;
};
