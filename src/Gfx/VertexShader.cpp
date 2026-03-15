#include "VertexShader.h"

#include <stdexcept>

#include "../Error.h"

VertexShader::VertexShader(Gfx &gfx) : Shader(gfx)
{
    auto byte_code = load(ShaderSource::VERTEX);

    HANDLE_GFX_ERR(device->CreateVertexShader(byte_code.data(), byte_code.size(), nullptr, shader.GetAddressOf()));

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,
         0},
        {"BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONE_COUNT", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"WORLD_XFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"WORLD_XFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"WORLD_XFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"WORLD_XFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"BONE_START", 0, DXGI_FORMAT_R32_UINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    };

    HANDLE_GFX_ERR(device->CreateInputLayout(layout, ARRAYSIZE(layout), byte_code.data(), byte_code.size(),
                                             input_layout.GetAddressOf()));
}

void VertexShader::set_buffer(VertexShader::BufferData *data)
{
}

void VertexShader::bind()
{
    ctx->IASetInputLayout(input_layout.Get());

    ctx->VSSetShader(shader.Get(), nullptr, 0);
}

void VertexShader::draw_indexed(UINT start_index, UINT index_count, UINT vertex_offset)
{
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    HANDLE_GFX_INFO(ctx->DrawIndexed(index_count, start_index, vertex_offset));
}

void VertexShader::draw_indexed_instanced(UINT start_index, UINT index_count, UINT start_instance, UINT instance_count,
                                          UINT vertex_offset)
{
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    HANDLE_GFX_INFO(ctx->DrawIndexedInstanced(index_count, instance_count, start_index, vertex_offset, start_instance));
}

// TextureVertexShader
TextureVertexShader::TextureVertexShader(Gfx &gfx) : Shader(gfx)
{
    auto byte_code = load(ShaderSource::VERTEX_TEXTURE);

    HANDLE_GFX_ERR(device->CreateVertexShader(byte_code.data(), byte_code.size(), nullptr, shader.GetAddressOf()));

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONE_INDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA,
         0},
        {"BONE_WEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"BONE_COUNT", 0, DXGI_FORMAT_R32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"WORLD_XFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"WORLD_XFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"WORLD_XFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"WORLD_XFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"BONE_START", 0, DXGI_FORMAT_R32_UINT, 1, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_INSTANCE_DATA, 1},
    };

    HANDLE_GFX_ERR(device->CreateInputLayout(layout, ARRAYSIZE(layout), byte_code.data(), byte_code.size(),
                                             input_layout.GetAddressOf()));
}

void TextureVertexShader::set_buffer(TextureVertexShader::BufferData *data)
{
}

void TextureVertexShader::bind()
{
    ctx->IASetInputLayout(input_layout.Get());

    ctx->VSSetShader(shader.Get(), nullptr, 0);
}

void TextureVertexShader::draw_indexed(UINT start_index, UINT index_count, UINT vertex_offset)
{
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    HANDLE_GFX_INFO(ctx->DrawIndexed(index_count, start_index, vertex_offset));
}

void TextureVertexShader::draw_indexed_instanced(UINT start_index, UINT index_count, UINT start_instance,
                                                 UINT instance_count, UINT vertex_offset)
{
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    HANDLE_GFX_INFO(ctx->DrawIndexedInstanced(index_count, instance_count, start_index, vertex_offset, start_instance));
}

SkyboxVertexShader::SkyboxVertexShader(Gfx &gfx) : Shader(gfx)
{
    auto byte_code = load(ShaderSource::VERTEX_SKYBOX);

    HANDLE_GFX_ERR(device->CreateVertexShader(byte_code.data(), byte_code.size(), nullptr, shader.GetAddressOf()));

    D3D11_INPUT_ELEMENT_DESC layout = {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA,
                                       0};

    HANDLE_GFX_ERR(
        device->CreateInputLayout(&layout, 1, byte_code.data(), byte_code.size(), input_layout.GetAddressOf()));
}

void SkyboxVertexShader::bind()
{
    ctx->IASetInputLayout(input_layout.Get());

    ctx->VSSetShader(shader.Get(), nullptr, 0);
}

ShadowMapVertexShader::ShadowMapVertexShader(Gfx &gfx) : Shader(gfx)
{
    auto byte_code = load(ShaderSource::VERTEX_SHADOW_MAP);

    HANDLE_GFX_ERR(device->CreateVertexShader(byte_code.data(), byte_code.size(), nullptr, shader.GetAddressOf()));

    byte_code = load(ShaderSource::VERTEX_TEX_SHADOW_MAP);

    HANDLE_GFX_ERR(device->CreateVertexShader(byte_code.data(), byte_code.size(), nullptr, shader_tex.GetAddressOf()));
}

void ShadowMapVertexShader::bind()
{
    throw std::runtime_error("ShadowMapVertexShader::bind requires an input_layout argument.");
}

void ShadowMapVertexShader::bind(ID3D11InputLayout *input_layout)
{
    ctx->IASetInputLayout(input_layout);

    ctx->VSSetShader(shader.Get(), nullptr, 0);

    ctx->PSSetShader(nullptr, nullptr, 0);
}

void ShadowMapVertexShader::bind_tex(ID3D11InputLayout *input_layout)
{
    ctx->IASetInputLayout(input_layout);

    ctx->VSSetShader(shader_tex.Get(), nullptr, 0);

    ctx->PSSetShader(nullptr, nullptr, 0);
}

void ShadowMapVertexShader::draw_indexed_instanced(UINT start_index, UINT index_count, UINT start_instance,
                                                   UINT instance_count, UINT vertex_offset)
{
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    HANDLE_GFX_INFO(ctx->DrawIndexedInstanced(index_count, instance_count, start_index, vertex_offset, start_instance));
}
