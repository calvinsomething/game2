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
    struct BufferData
    {
        struct
        {
            DirectX::XMMATRIX model;
        } transforms;
    };

  public:
    VertexShader(Gfx &gfx);

    ConstantBuffer<BufferData> constant_buffer;

    void bind() override;

    void update();

  private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
};
