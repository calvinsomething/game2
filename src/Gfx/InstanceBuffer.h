#pragma once

#include "Buffer.h"

#include <cstring>
#include <stdexcept>

#include "../Error.h"

class InstanceBuffer : public Buffer
{
  public:
    template <typename T, size_t N>
    InstanceBuffer(Gfx &gfx, T (*data)[N], UINT bind_flags, D3D11_USAGE usage, UINT cpu_access_flags = 0,
                   UINT pitch = 0, UINT slice_pitch = 0)
        : Buffer(gfx, data, bind_flags, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE)
    {
        stride = sizeof(T);
    }

    void bind() override
    {
        throw std::runtime_error("InstanceBuffer does not implement 'bind'.");
    }

    // TODO change signature? Or move to .cpp. Also factor out impl as it matches ConstantBuffer::write
    void update(void *data, size_t size)
    {
        D3D11_MAPPED_SUBRESOURCE mr;

        HANDLE_GFX_ERR(ctx->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr));

        std::memcpy(mr.pData, data, size);

        ctx->Unmap(buffer.Get(), 0);
    }
};
