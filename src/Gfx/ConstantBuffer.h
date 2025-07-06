#pragma once

#include "Buffer.h"

#include <cstring>

typedef void (*BindProc)(ID3D11DeviceContext *, ID3D11Buffer *);

class ConstantBuffer : public Buffer
{
  public:
    ConstantBuffer(Gfx &gfx, BindProc bind_proc, size_t byte_width);

    void bind() override;

    void write(void *data, size_t size);

    void resize(UINT byte_width);

  private:
    BindProc bind_proc;
};
