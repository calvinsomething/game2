#include "Camera.h"

#include <cmath>

using namespace DirectX;

Camera::Camera(Gfx &gfx)
    : focus_pos{0.0f, 0.0f, 0.0f}, up_dir{0.0f, 1.0f, 0.0f},
      constant_buffer(gfx, ConstantBuffer::Slot::CAMERA_BUFFER, &ConstantBuffer::bind_vs, sizeof(Camera::BufferData))
{
    increase_distance(20.0f);
    update_offset();

    buffer_data.camera_position = {0, 0, -distance};
    buffer_data.view_proj_xform =
        XMMatrixMultiplyTranspose(XMMatrixLookAtLH(buffer_data.camera_position, focus_pos, up_dir),
                                  XMMatrixPerspectiveLH(2.0f, 2.0f, 1.0f, 100.0f));
}

void Camera::update_offset()
{
    offset = XMVector3Transform(XMVECTOR{0, 0, -distance}, XMMatrixRotationRollPitchYaw(pitch, yaw, 0.0f));
}

void Camera::increase_pitch(float diff)
{
    pitch = max(min(pitch + diff, 1.57), -1.57); // max/min just less than 90 degrees from neutral
}

void Camera::increase_yaw(float diff)
{
    yaw = fmod(yaw + diff, XM_2PI);
}

XMVECTOR Camera::get_direction()
{
    return -XMVector3Normalize(offset);
}

void Camera::increase_distance(float diff)
{
    float next = distance + diff;
    distance = next < max_distance ? next > min_distance ? next : min_distance : max_distance;
}

void Camera::update(Controls controls, DirectX::XMFLOAT3 focus_position)
{
    if (controls.pitch_delta || controls.yaw_delta || controls.distance_delta)
    {
        if (controls.pitch_delta)
        {
            increase_pitch(controls.pitch_delta);
        }
        if (controls.yaw_delta)
        {
            increase_yaw(controls.yaw_delta);
        }
        if (controls.distance_delta)
        {
            increase_distance(controls.distance_delta);
        }

        update_offset();
    }

    focus_pos = XMVectorSet(focus_position.x, focus_position.y, focus_position.z, 1.0f);

    buffer_data.camera_position = XMVectorAdd(focus_pos, offset);

    buffer_data.view_proj_xform =
        XMMatrixMultiplyTranspose(XMMatrixLookAtLH(buffer_data.camera_position, focus_pos, up_dir),
                                  XMMatrixPerspectiveLH(2.0f, 2.0f, 1.0f, 100.0f));

    constant_buffer.write(&buffer_data, sizeof(buffer_data));

    constant_buffer.bind();
}
