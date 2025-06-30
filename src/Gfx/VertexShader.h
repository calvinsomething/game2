#pragma once

#include "Shader.h"

#include <DirectXMath.h>

struct Vec3
{
    float x, y, z;
};

struct Vec4
{
    float x, y, z, w;
};

struct Vertex
{
    DirectX::XMFLOAT4 position, color;
};

class VertexShader : public Shader
{
  public:
    VertexShader(Gfx &gfx);

    void bind() override;

    void update();

  private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
};
