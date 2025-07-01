#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Gfx.h"

class Buffer
{
  public:
    Buffer(Gfx &gfx) : device(gfx.device.Get()), ctx(gfx.ctx.Get())
    {
    }
    virtual void bind() = 0;

  protected:
    ID3D11Device *device;
    ID3D11DeviceContext *ctx;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};
