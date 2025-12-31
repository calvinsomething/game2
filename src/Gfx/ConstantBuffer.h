#pragma once

#include "Buffer.h"

#include <cstring>

typedef void (*ConstantBufferBindProc)(ID3D11DeviceContext *, ID3D11Buffer *, size_t slot_index);

class ConstantBuffer : public Buffer
{
  public:
    enum Slot : size_t
    {
        CAMERA_BUFFER,
        LIGHTING_BUFFER,
        MESH_BUFFER,
    };

    template <typename T>
    ConstantBuffer(Gfx &gfx, size_t slot_index, ConstantBufferBindProc bind_proc, T &data)
        : Buffer(gfx, &data, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DEFAULT, 0), slot_index(slot_index),
          bind_proc(bind_proc)
    {
    }

    // byte_width must be multiple of 16
    ConstantBuffer(Gfx &gfx, size_t slot_index, ConstantBufferBindProc bind_proc, UINT byte_width);

    void bind() override;

    void write(void *data, size_t size);

    void resize(UINT byte_width);

    static void bind_vs(ID3D11DeviceContext *ctx, ID3D11Buffer *buffer, size_t slot_index);
    static void bind_ps(ID3D11DeviceContext *ctx, ID3D11Buffer *buffer, size_t slot_index);
    static void bind_vs_and_ps(ID3D11DeviceContext *ctx, ID3D11Buffer *buffer, size_t slot_index);

  private:
    size_t slot_index;

    ConstantBufferBindProc bind_proc;
};
