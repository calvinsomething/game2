#pragma once

#include "Gfx.h"

#include <wrl.h>

#include <string>

class Shader : protected GfxAccess
{
  public:
    enum ShaderSource : size_t
    {
        VERTEX,
        VERTEX_TEXTURE,
        VERTEX_SKYBOX,
        PIXEL,
        PIXEL_TEXTURE,
        PIXEL_SKYBOX,
        COMPUTE_SKYBOX,
        VERTEX_SHADOW_MAP,
        VERTEX_TEX_SHADOW_MAP,
    };

    virtual void bind() = 0;

    ID3D11InputLayout *get_input_layout();

  protected:
    std::string load(ShaderSource source);

    Shader(Gfx &gfx);

    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
};
