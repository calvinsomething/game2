#include "Buffer.h"

#include "../Error.h"

Buffer::Buffer(Gfx &gfx, UINT byte_width, UINT bind_flags, D3D11_USAGE usage, UINT cpu_access_flags, UINT pitch,
               UINT slice_pitch)
    : GfxAccess(gfx)
{
    init(nullptr, byte_width, bind_flags, usage, cpu_access_flags, pitch, slice_pitch);
}

void Buffer::init(void *data, UINT byte_width, UINT bind_flags, D3D11_USAGE usage, UINT cpu_access_flags, UINT pitch,
                  UINT slice_pitch)
{
    D3D11_BUFFER_DESC bd = {};
    bd.BindFlags = bind_flags;
    bd.ByteWidth = byte_width;
    bd.Usage = usage;
    bd.CPUAccessFlags = cpu_access_flags;
    bd.MiscFlags = 0;

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
        &bd, p_srd, buffer.ReleaseAndGetAddressOf())); // release so init can be used to update buffer structure
}
