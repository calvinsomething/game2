#pragma once

#include "Buffer.h"

class VertexBuffer : public Buffer
{
  public:
    template <typename T>
    VertexBuffer(Gfx &gfx, T &vertices) : Buffer(gfx, &vertices, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DEFAULT)
    {
    }

    void bind() override;
};
