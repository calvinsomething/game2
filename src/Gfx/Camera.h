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
    DirectX::FXMVECTOR eye_pos;
    DirectX::FXMVECTOR focus_pos;
    DirectX::FXMVECTOR up_dir;

    ConstantBuffer constant_buffer;

    BufferData buffer_data;
};
