#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Gfx.h"

class Buffer
{
  public:
    virtual void bind() = 0;

  protected:
    Buffer(Gfx &gfx) : device(gfx.device.Get()), ctx(gfx.ctx.Get())
    {
    }

    ID3D11Device *device;
    ID3D11DeviceContext *ctx;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};
