#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Gfx.h"

class Buffer
{
  public:
    virtual void bind() = 0;

    template <typename T>
    Buffer(Gfx &gfx, T *data, UINT bind_flags, D3D11_USAGE usage, UINT cpu_access_flags = 0, UINT pitch = 0,
           UINT slice_pitch = 0)
        : device(gfx.device.Get()), ctx(gfx.ctx.Get())
    {
        init(data, sizeof(T), bind_flags, usage, cpu_access_flags, pitch, slice_pitch);
    }

    Buffer(Gfx &gfx, UINT byte_width, UINT bind_flags, D3D11_USAGE usage, UINT cpu_access_flags = 0, UINT pitch = 0,
           UINT slice_pitch = 0);

  protected:
    ID3D11Device *device;
    ID3D11DeviceContext *ctx;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

    void init(void *data, UINT byte_width, UINT bind_flags, D3D11_USAGE usage, UINT cpu_access_flags = 0,
              UINT pitch = 0, UINT slice_pitch = 0);
};
