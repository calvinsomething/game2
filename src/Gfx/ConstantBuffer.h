#pragma once

#include "Buffer.h"

#include <utility>

#include "../Error.h"

typedef void (*BindProc)(ID3D11DeviceContext *, ID3D11Buffer *);

template <typename T> class ConstantBuffer : public Buffer
{
  public:
    ConstantBuffer(Gfx &gfx, T &&data, BindProc bind_proc) : Buffer(gfx), data(std::move(data)), bind_proc(bind_proc)
    {
        D3D11_BUFFER_DESC bd = {};
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.ByteWidth = sizeof(T);
        bd.Usage = D3D11_USAGE_DYNAMIC;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;

        D3D11_SUBRESOURCE_DATA sd = {};
        sd.pSysMem = &data;
        sd.SysMemPitch = 0;
        sd.SysMemSlicePitch = 0;

        HANDLE_GFX_ERR(device->CreateBuffer(&bd, &sd, buffer.GetAddressOf()));
    }

    void bind() override
    {
        if (bind_proc)
        {
            bind_proc(ctx, buffer.Get());
        }
    }

  private:
    T data;

    BindProc bind_proc;
};
