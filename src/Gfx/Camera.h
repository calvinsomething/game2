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

    void update(DirectX::XMFLOAT3 position);

    void increase_elevation(float diff);
    void increase_azimuth(float diff);
    void increase_distance(float diff);

  private:
    float elevation, azimuth, distance, max_distance = 20.0f, min_distance = 5.0f;

    DirectX::XMVECTOR eye_pos;
    DirectX::XMVECTOR focus_pos;
    DirectX::XMVECTOR up_dir;

    ConstantBuffer constant_buffer;

    BufferData buffer_data;
};
