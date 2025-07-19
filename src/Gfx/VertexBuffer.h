#pragma once

#include "Buffer.h"
#include "Buffers.h"

#include "VertexShader.h"

class VertexBuffer : public Buffer
{
  public:
    template <typename T>
    VertexBuffer(Gfx &gfx, T &vertices) : Buffer(gfx, &vertices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT)
    {
        stride = sizeof(Vertex); // assuming no padding/offset
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
