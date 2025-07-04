#pragma once

#include "Shader.h"

#include <DirectXMath.h>

#include "ConstantBuffer.h"

struct Vertex
{
    DirectX::XMFLOAT4 position, color;
};

class VertexShader : public Shader
{
  public:
    struct BufferData
    {
        struct
        {
            DirectX::XMMATRIX model;
        } transforms;
    };

    VertexShader(Gfx &gfx);

    ConstantBuffer constant_buffer;

    void bind() override;

    void set_model_transform(DirectX::XMFLOAT3X3 mat);

    void update();

  private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
};
