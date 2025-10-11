#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Gfx.h"

class Buffer : protected GfxAccess
{
    friend class Buffers;

  public:
    virtual void bind() = 0;

  protected:
    Buffer(Gfx &gfx);

    template <typename T>
    Buffer(Gfx &gfx, T *data, UINT bind_flags, D3D11_USAGE usage, UINT cpu_access_flags = 0, UINT pitch = 0,
           UINT slice_pitch = 0, D3D11_RESOURCE_MISC_FLAG misc_flags = D3D11_RESOURCE_MISC_FLAG(0))
        : GfxAccess(gfx)
    {
        init(data, sizeof(T), bind_flags, usage, cpu_access_flags, pitch, slice_pitch, misc_flags);
    }

    Buffer(Gfx &gfx, UINT byte_width, UINT bind_flags, D3D11_USAGE usage, UINT cpu_access_flags = 0, UINT pitch = 0,
           UINT slice_pitch = 0);

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

    UINT stride = 0;
    UINT offset = 0;

    void init(void *data, UINT byte_width, UINT bind_flags, D3D11_USAGE usage, UINT cpu_access_flags = 0,
              UINT pitch = 0, UINT slice_pitch = 0, UINT structured_byte_stride = 0,
              D3D11_RESOURCE_MISC_FLAG misc_flags = D3D11_RESOURCE_MISC_FLAG(0));
};
