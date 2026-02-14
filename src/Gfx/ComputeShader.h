#pragma once

#include "Shader.h"

class ComputeShader : public Shader
{
  public:
    enum Slot : size_t
    {
        OUTPUT_TEXTURE_2D,
        INPUT_BUFFER = 0,
    };

    ComputeShader(Gfx &gfx, ShaderSource source);

    void bind() override;

  private:
    Microsoft::WRL::ComPtr<ID3D11ComputeShader> shader;
};
