#pragma once

#include "Shader.h"

class PixelShader : public Shader
{
  public:
    PixelShader(Gfx &gfx);

    void bind() override;

  private:
    Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
};
