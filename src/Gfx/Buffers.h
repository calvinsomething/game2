#pragma once

#include "Buffer.h"

class Buffers : protected GfxAccess
{
  public:
    template <size_t N> Buffers(Gfx &gfx, Buffer *(&buffers)[N]) : GfxAccess(gfx), buffers{}, strides{}, offsets{}
    {
        static_assert(N <= 10, "Buffers class cannot be initialized with more than 10 buffers.");

        count = N;

        for (size_t i = 0; i < count; i++)
        {
            Buffers::buffers[i] = buffers[i]->buffer.Get();
            strides[i] = buffers[i]->stride;
            offsets[i] = buffers[i]->offset;
        }
    }

    virtual void bind() = 0;

  protected:
    ID3D11Buffer *buffers[10];
    UINT strides[10];
    UINT offsets[10];

    UINT count;
};
