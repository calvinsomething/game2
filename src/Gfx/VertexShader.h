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
    void draw_indexed_instanced(UINT start_index, UINT index_count, UINT start_instance, UINT instance_count,
                                UINT vertex_offset = 0);

  private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
};

// TextureVertexShader
struct TextureVertex
{
    DirectX::XMFLOAT4 position;
    DirectX::XMFLOAT2 texture_coordinate;
};

class TextureVertexShader : public Shader
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

    TextureVertexShader(Gfx &gfx);

    void bind() override;

    void set_buffer(BufferData *data);

    void draw_indexed(UINT count);
    void draw_indexed_instanced(UINT start_index, UINT index_count, UINT start_instance, UINT instance_count,
                                UINT vertex_offset = 0);

  private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
};
