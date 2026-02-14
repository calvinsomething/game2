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
    };

    virtual void bind() = 0;

  protected:
    std::string load(ShaderSource source);

    Shader(Gfx &gfx);

    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
};
