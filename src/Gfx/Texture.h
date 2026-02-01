#pragma once

#include <d3d11.h>
#include <wrl.h>

#include "Gfx.h"
#include "assimp/texture.h"

class Texture : protected GfxAccess
{
  public:
    Texture(Gfx &gfx);

    ID3D11SamplerState *get_sampler_state();
    ID3D11ShaderResourceView *get_view();

  protected:
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> view;

    static Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state;
    void init_sampler_state();
};

class Texture2D : public Texture
{
  public:
    Texture2D(Gfx &gfx);
    Texture2D(Gfx &gfx, const wchar_t *file_name);

    void load(const wchar_t *file_name);
    void load(const aiTexture *ai_texture);

    void load_cube_from_hdr(const char *file_name);

  private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
};
