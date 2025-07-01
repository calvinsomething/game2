#pragma once

#include "Buffer.h"

#include <cstdint>

#include "../Error.h"

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

    void bind() override;
};
