#include "Camera.h"

using namespace DirectX;

Camera::Camera(Gfx &gfx)
    : azimuth(), distance(20.0f), focus_pos{0.0f, 0.0f, 0.0f}, up_dir{0.0f, 1.0f, 0.0f},
      constant_buffer(gfx, ConstantBuffer::Slot::CAMERA_BUFFER, &ConstantBuffer::bind_vs, sizeof(Camera::BufferData))
{
    buffer_data.camera_position = {0, 0, -distance};
    buffer_data.view_proj_xform =
        XMMatrixMultiplyTranspose(XMMatrixLookAtLH(buffer_data.camera_position, focus_pos, up_dir),
                                  XMMatrixPerspectiveLH(2.0f, 2.0f, 1.0f, 100.0f));
}

void Camera::increase_elevation(float diff)
{
}

void Camera::increase_azimuth(float diff)
{
    constexpr float step = 0.2f, factor = 1000000.0f, factor_inv = 1.0f / factor;
    constexpr int pi_x_factor = static_cast<int>(XM_PI * factor);

    azimuth += float(int(diff * step * factor) % pi_x_factor) * factor_inv;
}

void Camera::increase_distance(float diff)
{
    float next = distance + diff;
    distance = next < max_distance ? next > min_distance ? next : min_distance : max_distance;
}

void Camera::update(DirectX::XMFLOAT3 position)
{
    focus_pos = XMVectorSet(position.x, position.y, position.z, 1.0f);

    XMVECTOR offset =
        XMVector3Transform(XMVECTOR{0, 0, -distance}, XMMatrixRotationRollPitchYaw(-elevation, azimuth, 0.0f));

    buffer_data.camera_position = XMVectorAdd(focus_pos, offset);

    buffer_data.view_proj_xform =
        XMMatrixMultiplyTranspose(XMMatrixLookAtLH(buffer_data.camera_position, focus_pos, up_dir),
                                  XMMatrixPerspectiveLH(2.0f, 2.0f, 1.0f, 100.0f));

    constant_buffer.write(&buffer_data, sizeof(buffer_data));

    constant_buffer.bind();
}
