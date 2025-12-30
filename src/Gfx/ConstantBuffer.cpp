#include "ConstantBuffer.h"

#include "../Error.h"

ConstantBuffer::ConstantBuffer(Gfx &gfx, size_t slot_index, ConstantBufferBindProc bind_proc, UINT byte_width)
    : Buffer(gfx, byte_width, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE),
      slot_index(slot_index), bind_proc(bind_proc)
{
}

void ConstantBuffer::bind()
{
    if (bind_proc)
    {
        bind_proc(ctx, buffer.Get(), slot_index);
    }
}

void ConstantBuffer::write(void *data, size_t size)
{
    D3D11_MAPPED_SUBRESOURCE mr;

    HANDLE_GFX_ERR(ctx->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr));

    std::memcpy(mr.pData, data, size);

    ctx->Unmap(buffer.Get(), 0);
}

void ConstantBuffer::resize(UINT byte_width)
{
    init(nullptr, byte_width, D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}
