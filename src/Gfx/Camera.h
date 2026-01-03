#pragma once

#include "ConstantBuffer.h"

class Camera
{
  public:
    struct BufferData
    {
        DirectX::XMVECTOR camera_position;
        DirectX::XMMATRIX view_proj_xform;
    };

    Camera(Gfx &gfx);

    void update(DirectX::XMFLOAT3 position);

    void increase_pitch(float diff);
    void increase_yaw(float diff);
    void increase_distance(float diff);

  private:
    float pitch = 0, yaw = 0, distance = 0, max_distance = 20.0f, min_distance = 5.0f;

    DirectX::XMVECTOR focus_pos;
    DirectX::XMVECTOR up_dir;

    ConstantBuffer constant_buffer;

    BufferData buffer_data;
};
