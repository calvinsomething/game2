#include "VertexShader.h"

#include "../Error.h"

VertexShader::VertexShader(Gfx &gfx) : Shader(gfx)
{
    auto byte_code = load("vs.cso");

    HANDLE_GFX_ERR(device->CreateVertexShader(byte_code.data(), byte_code.size(), nullptr, &shader));

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"MODEL_XFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"MODEL_XFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"MODEL_XFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_INSTANCE_DATA, 1},
        {"MODEL_XFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_APPEND_ALIGNED_ELEMENT,
         D3D11_INPUT_PER_INSTANCE_DATA, 1},
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

void VertexShader::draw_indexed(UINT count)
{
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    HANDLE_GFX_INFO(ctx->DrawIndexed(count, 0, 0));
}

void VertexShader::draw_indexeded_instanced(UINT index_count, UINT instance_count, UINT start_index)
{
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    HANDLE_GFX_INFO(ctx->DrawIndexedInstanced(index_count, instance_count, start_index, 0, 0));
}
