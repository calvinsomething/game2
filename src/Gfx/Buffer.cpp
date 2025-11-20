#include "Buffer.h"

#include "../Error.h"

Buffer::Buffer(Gfx &gfx) : GfxAccess(gfx)
{
}

Buffer::Buffer(Gfx &gfx, UINT byte_width, UINT bind_flags, D3D11_USAGE usage, UINT cpu_access_flags, UINT pitch,
               UINT slice_pitch)
    : GfxAccess(gfx)
{
    init(nullptr, byte_width, bind_flags, usage, cpu_access_flags, pitch, slice_pitch);
}

void Buffer::init(void *data, UINT byte_width, UINT bind_flags, D3D11_USAGE usage, UINT cpu_access_flags, UINT pitch,
                  UINT slice_pitch, UINT structured_byte_stride, D3D11_RESOURCE_MISC_FLAG misc_flags)
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = bind_flags;
    bd.ByteWidth = byte_width;
    bd.Usage = usage;
    bd.CPUAccessFlags = cpu_access_flags;
    bd.StructureByteStride = structured_byte_stride;
    bd.MiscFlags = misc_flags;

    D3D11_SUBRESOURCE_DATA *p_srd = 0;
    D3D11_SUBRESOURCE_DATA srd = {};

    if (data)
    {
        srd.pSysMem = data;
        srd.SysMemPitch = pitch;
        srd.SysMemSlicePitch = slice_pitch;

        p_srd = &srd;
    }

    HANDLE_GFX_ERR(device->CreateBuffer(
        &bd, p_srd, buffer.ReleaseAndGetAddressOf())); // release so init can be used to write over buffer structure
}

void Buffer::start_batch_update()
{
    HANDLE_GFX_ERR(ctx->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr));

    resource_is_mapped = true;
}

void Buffer::end_batch_update()
{
    ctx->Unmap(buffer.Get(), 0);

    resource_is_mapped = false;
}

void Buffer::update(void *data, size_t size, size_t offset)
{
    if (resource_is_mapped)
    {
        std::memcpy(static_cast<char *>(mr.pData) + offset, data, size);
    }
    else
    {
        HANDLE_GFX_ERR(ctx->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mr));

        std::memcpy(static_cast<char *>(mr.pData) + offset, data, size);

        ctx->Unmap(buffer.Get(), 0);
    }
}
