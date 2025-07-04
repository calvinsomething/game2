#pragma once

#include "Buffer.h"

#include <cstring>

#include "../Error.h"

typedef void (*BindProc)(ID3D11DeviceContext *, ID3D11Buffer *);

class ConstantBuffer : public Buffer
{
  public:
    ConstantBuffer(Gfx &gfx, BindProc bind_proc, size_t byte_width)
        : Buffer(gfx, byte_width, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE),
          bind_proc(bind_proc)
    {
    }

    void bind() override
    {
        if (bind_proc)
        {
            bind_proc(ctx, buffer.Get());
        }
    }

    void write(void *data, size_t size)
    {
        D3D11_MAPPED_SUBRESOURCE mr;

        HANDLE_GFX_ERR(ctx->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr));

        std::memcpy(mr.pData, data, size);

        ctx->Unmap(buffer.Get(), 0);
    }

  private:
    BindProc bind_proc;
};
