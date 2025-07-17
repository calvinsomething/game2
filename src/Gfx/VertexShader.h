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

    // ConstantBuffer constant_buffer;

    void bind() override;

    void set_buffer(BufferData *data);

    void draw_indexed(UINT count);

  private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
};
