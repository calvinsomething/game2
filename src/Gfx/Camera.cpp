#include "Camera.h"

using namespace DirectX;

void bind_proc(ID3D11DeviceContext *ctx, ID3D11Buffer *buffer)
{
    ctx->VSSetConstantBuffers(0, 1, &buffer);
}

Camera::Camera(Gfx &gfx)
    : distance(20.0f), eye_pos{0, 0, -distance}, focus_pos{0.0f, 0.0f, 0.0f}, up_dir{0.0f, 1.0f, 0.0f},
      constant_buffer(gfx, &bind_proc, sizeof(Camera::BufferData)),
      buffer_data{XMMatrixMultiplyTranspose(XMMatrixLookAtLH(eye_pos, focus_pos, up_dir),
                                            XMMatrixPerspectiveLH(2.0f, 2.0f, 1.0f, 100.0f))}
{
}

void Camera::increase_elevation(float diff)
{
}

void Camera::increase_azimuth(float diff)
{
    constexpr float step = 0.2f, bil = 1000000000.0f, bil_inv = 1.0f / bil;
    static const int pi_x_bil = XM_PI * bil;

    azimuth += float(int(diff * step * bil) % pi_x_bil) * bil_inv;
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

    eye_pos = XMVectorAdd(focus_pos, offset);

    buffer_data.view_proj_xform = XMMatrixMultiplyTranspose(XMMatrixLookAtLH(eye_pos, focus_pos, up_dir),
                                                            XMMatrixPerspectiveLH(2.0f, 2.0f, 1.0f, 100.0f));

    constant_buffer.write(&buffer_data, sizeof(buffer_data));

    constant_buffer.bind();
}
