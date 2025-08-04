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
    TexturePixelShader(Gfx &gfx, Texture &texture);

    void bind() override;

  private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;

    Texture &texture;
};
