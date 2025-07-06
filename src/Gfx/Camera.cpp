#include "Camera.h"

void bind_proc(ID3D11DeviceContext *ctx, ID3D11Buffer *buffer)
{
    ctx->VSSetConstantBuffers(0, 1, &buffer);
}

Camera::Camera(Gfx &gfx)
    : constant_buffer(gfx, &bind_proc, sizeof(Camera::BufferData)),
      buffer_data{DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveLH(2.0f, 2.0f, 1.0f, 10.0f))}
{
}

void Camera::update()
{
    constant_buffer.write(&buffer_data, sizeof(buffer_data));

    constant_buffer.bind();
}
