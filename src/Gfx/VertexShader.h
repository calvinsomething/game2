#pragma once

#include "Shader.h"

#include <DirectXMath.h>

struct ShaderBoneData
{
    unsigned indices[4] = {};
    float weights[4] = {};
    unsigned count = 0;
};

struct Vertex
{
    DirectX::XMFLOAT3 position;
    // DirectX::XMFLOAT3 normal;
    DirectX::XMFLOAT4 color;
    ShaderBoneData bone;
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

    void draw_indexed(UINT start_index, UINT index_count, UINT vertex_offset);
    void draw_indexed_instanced(UINT start_index, UINT index_count, UINT start_instance, UINT instance_count,
                                UINT vertex_offset = 0);

  private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
};

// TextureVertexShader
struct TextureCoordinates
{
    DirectX::XMFLOAT2 diffuse;
    DirectX::XMFLOAT2 normal;
};

struct TextureVertex
{
    DirectX::XMFLOAT3 position;
    // DirectX::XMFLOAT3 normal;
    TextureCoordinates texture_coordinates;
    ShaderBoneData bone;
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

    void draw_indexed(UINT start_index, UINT index_count, UINT vertex_offset);
    void draw_indexed_instanced(UINT start_index, UINT index_count, UINT start_instance, UINT instance_count,
                                UINT vertex_offset = 0);

  private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
};
