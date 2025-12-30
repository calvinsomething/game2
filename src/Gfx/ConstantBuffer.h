#pragma once

#include "Buffer.h"

#include <cstring>

typedef void (*ConstantBufferBindProc)(ID3D11DeviceContext *, ID3D11Buffer *, size_t slot_index);

class ConstantBuffer : public Buffer
{
  public:
    enum Slot : size_t
    {
        GLOBAL_BUFFER,
        MESH_BUFFER,
    };

    // byte_width must be multiple of 16
    ConstantBuffer(Gfx &gfx, size_t slot_index, ConstantBufferBindProc bind_proc, UINT byte_width);

    void bind() override;

    void write(void *data, size_t size);

    void resize(UINT byte_width);

  private:
    size_t slot_index;

    ConstantBufferBindProc bind_proc;
};
