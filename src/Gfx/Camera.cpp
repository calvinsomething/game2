#include "Camera.h"

using namespace DirectX;

void bind_proc(ID3D11DeviceContext *ctx, ID3D11Buffer *buffer)
{
    ctx->VSSetConstantBuffers(0, 1, &buffer);
}

Camera::Camera(Gfx &gfx)
    : eye_pos{0, 0, -20.0f}, focus_pos{0.0f, 0.0f, 0.0f}, up_dir{0.0f, 1.0f, 0.0f},
      constant_buffer(gfx, &bind_proc, sizeof(Camera::BufferData)),
      buffer_data{XMMatrixMultiplyTranspose(XMMatrixLookAtLH(eye_pos, focus_pos, up_dir),
                                            XMMatrixPerspectiveLH(2.0f, 2.0f, 1.0f, 100.0f))}
{
}

void Camera::move(DirectX::FXMVECTOR diff)
{
    eye_pos = XMVectorAdd(eye_pos, diff);
}

void Camera::update()
{
    buffer_data.view_proj_xform = XMMatrixMultiplyTranspose(XMMatrixLookAtLH(eye_pos, focus_pos, up_dir),
                                                            XMMatrixPerspectiveLH(2.0f, 2.0f, 1.0f, 100.0f));

    constant_buffer.write(&buffer_data, sizeof(buffer_data));

    constant_buffer.bind();
}
