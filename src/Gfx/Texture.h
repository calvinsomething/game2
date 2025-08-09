#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Gfx.h"

class Texture : private GfxAccess
{
  public:
    Texture(Gfx &gfx);

    ID3D11SamplerState *get_sampler();
    ID3D11ShaderResourceView *get_view();

    void load();

  private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
};
