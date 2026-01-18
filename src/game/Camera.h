#pragma once

#include "../gfx/ConstantBuffer.h"

class Camera
{
  public:
    struct BufferData
    {
        DirectX::XMVECTOR camera_position;
        DirectX::XMMATRIX view_proj_xform;
    };

    struct Controls
    {
        float pitch_delta = 0, yaw_delta = 0, distance_delta = 0;
    };

    Camera(Gfx &gfx);

    void update(Controls controls, DirectX::XMFLOAT3 focus_position);

    DirectX::XMVECTOR get_direction();

  private:
    float pitch = 0, yaw = 0, distance = 0, max_distance = 20.0f, min_distance = 3.0f;

    DirectX::XMVECTOR focus_pos;
    DirectX::XMVECTOR up_dir;

    DirectX::XMVECTOR offset;

    ConstantBuffer constant_buffer;

    BufferData buffer_data;

    void increase_pitch(float diff);
    void increase_yaw(float diff);
    void increase_distance(float diff);

    void update_offset();
};
