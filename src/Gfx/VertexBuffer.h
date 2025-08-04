#pragma once

#include "Buffer.h"
#include "Buffers.h"

class VertexBuffer : public Buffer
{
  public:
    template <typename T, size_t N>
    VertexBuffer(Gfx &gfx, T (&vertices)[N]) : Buffer(gfx, &vertices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT)
    {
        stride = sizeof(T); // assuming no padding/offset
    }

    void bind() override;
};

class VertexBuffers : public Buffers
{
  public:
    template <size_t N> VertexBuffers(Gfx &gfx, Buffer *(&buffers)[N]) : Buffers(gfx, buffers)
    {
    }

    void bind() override;
};
