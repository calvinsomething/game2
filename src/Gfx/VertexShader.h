#pragma once

#include "Shader.h"

#include <DirectXMath.h>

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

    void bind() override;

    void set_buffer(BufferData *data);

    void draw_indexed(UINT count);
    void draw_indexeded_instanced(UINT index_count, UINT instance_count, UINT start_index);

  private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
};
