#pragma once

#include "Shader.h"

#include "../models/Material.h"

class PixelShader : public Shader
{
  public:
    PixelShader(Gfx &gfx);

    void bind() override;

    virtual void set_material(const Material &material);

  protected:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};

class TexturePixelShader : public PixelShader
{
  public:
    TexturePixelShader(Gfx &gfx);

    void set_material(const Material &material) override;
};
