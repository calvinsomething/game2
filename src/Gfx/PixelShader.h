#pragma once

#include "ComputeShader.h"
#include "Shader.h"

#include "../gfx/Texture.h"
#include "../util.h"

class PixelShader : public Shader
{
  public:
    PixelShader(Gfx &gfx);

    virtual void bind() override;

  protected:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};

class TexturePixelShader : public PixelShader
{
  public:
    TexturePixelShader(Gfx &gfx);

    void set_textures(StdVector<Texture2D> &textures);
    // void set_material(const Material &material);
};

class SkyboxPixelShader : public PixelShader
{
  public:
    SkyboxPixelShader(Gfx &gfx);

    void load_texture(ComputeShader &cs, size_t cube_span);

    void bind() override;

  private:
    Texture2D texture_array;
};
