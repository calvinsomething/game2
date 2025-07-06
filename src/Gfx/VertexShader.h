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
            DirectX::XMMATRIX world;
        } transforms;
    };

    VertexShader(Gfx &gfx);

    ConstantBuffer constant_buffer;

    void bind() override;

    void set_transforms(DirectX::XMFLOAT3X3 model, DirectX::XMFLOAT3X3 world);

  private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;

    BufferData buffer_data;
};
