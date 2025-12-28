#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Gfx.h"
#include "assimp/texture.h"

class Texture : private GfxAccess
{
  public:
    Texture(Gfx &gfx, const wchar_t *file_name);
    Texture(Gfx &gfx, const aiTexture *ai_texture);

    ID3D11SamplerState *get_sampler_state();
    ID3D11ShaderResourceView *get_view();

    void load(const wchar_t *file_name);
    void load(const aiTexture *ai_texture);

  private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;
};
