#pragma once

#include "Shader.h"

#include "Texture.h"

class PixelShader : public Shader
{
  public:
    PixelShader(Gfx &gfx);

    void bind() override;

  private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};

class TexturePixelShader : public Shader
{
  public:
    TexturePixelShader(Gfx &gfx);

    void set_texture(Texture *t);

    void bind() override;
    void bind(Texture *texture);

  private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;

    Texture *texture = 0;
};
