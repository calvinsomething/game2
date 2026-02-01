#pragma once

#include "Buffer.h"

#include "../util.h"

class IndexBuffer : public Buffer
{
  public:
    template <typename T, size_t N>
    IndexBuffer(Gfx &gfx, T (&indices)[N]) : Buffer(gfx, &indices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT)
    {
        stride = sizeof(T); // assuming no padding/offset
    }

    template <typename T> IndexBuffer(Gfx &gfx, T *indices, size_t size) : Buffer(gfx)
    {
        stride = sizeof(T); // assuming no padding/offset

        init(indices, size * stride, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT);
    }

    template <typename T> IndexBuffer(Gfx &gfx, StdVector<T> &indices) : Buffer(gfx)
    {
        stride = sizeof(T); // assuming no padding/offset

        init(indices.data(), UINT(indices.size()) * stride, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT);
    }

    void bind() override;
};
