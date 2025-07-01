#pragma once

#include "Buffer.h"

#include "VertexShader.h"

class VertexBuffer : public Buffer
{
  public:
    template <size_t N> VertexBuffer(Gfx &gfx, Vertex (&vertices)[N]) : Buffer(gfx)
    {
        D3D11_BUFFER_DESC bd = {};
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.ByteWidth = sizeof(vertices);
        bd.Usage = D3D11_USAGE_DEFAULT; // D3D11_USAGE_DYNAMIC
        bd.CPUAccessFlags = 0;          // D3D11_CPU_ACCESS_WRITE
        bd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = vertices;
        data.SysMemPitch = 0;
        data.SysMemSlicePitch = 0;

        HANDLE_GFX_ERR(device->CreateBuffer(&bd, &data, buffer.GetAddressOf()));
    }

    void bind() override;
};
