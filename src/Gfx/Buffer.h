#pragma once

#include <d3d11.h>

#include <cstdint>

#include "../Error.h"
#include "Gfx.h"

class Buffer
{
  public:
    Buffer(Gfx &gfx) : device(gfx.device), ctx(gfx.ctx)
    {
    }
    virtual void bind() = 0;

  protected:
    ID3D11Device *device;
    ID3D11DeviceContext *ctx;
    ID3D11Buffer *buffer;
};

class IndexBuffer : public Buffer
{
  public:
    template <size_t N> IndexBuffer(Gfx &gfx, uint32_t (&indices)[N]) : Buffer(gfx)
    {
        D3D11_BUFFER_DESC bd = {};
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.ByteWidth = sizeof(indices);
        // bd.Usage = D3D11_USAGE_DEFAULT;
        // bd.CPUAccessFlags  = 0; // D3D11_CPU_ACCESS_WRITE
        // bd.MiscFlags       = 0;

        D3D11_SUBRESOURCE_DATA data = {};
        data.pSysMem = indices;
        // data.SysMemPitch = 0;
        // data.SysMemSlicePitch = 0;

        HANDLE_GFX_ERR(device->CreateBuffer(&bd, &data, &buffer));
    }

    void bind() override
    {
        ctx->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
    }
};
