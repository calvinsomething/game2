#pragma once

#include "Buffer.h"

class IndexBuffer : public Buffer
{
  public:
    template <typename T>
    IndexBuffer(Gfx &gfx, T &indices) : Buffer(gfx, &indices, D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_DEFAULT)
    {
    }

    void bind() override;
};
