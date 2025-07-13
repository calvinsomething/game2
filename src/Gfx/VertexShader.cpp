#include "VertexShader.h"

#include "../Error.h"

static void bind_proc(ID3D11DeviceContext *ctx, ID3D11Buffer *buffer)
{
    ctx->VSSetConstantBuffers(1, 1, &buffer);
}

VertexShader::VertexShader(Gfx &gfx) : Shader(gfx), constant_buffer(gfx, &bind_proc, sizeof(VertexShader::BufferData))
{
    auto byte_code = load("vs.cso");

    HANDLE_GFX_ERR(device->CreateVertexShader(byte_code.data(), byte_code.size(), nullptr, &shader));

    D3D11_INPUT_ELEMENT_DESC layout[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
    };

    HANDLE_GFX_ERR(device->CreateInputLayout(layout, sizeof(layout) / sizeof(layout[0]), byte_code.data(),
                                             byte_code.size(), input_layout.GetAddressOf()));
}

void VertexShader::set_buffer(VertexShader::BufferData *data)
{
    constant_buffer.write(data, sizeof(*data));
}

void VertexShader::bind()
{
    constant_buffer.bind();

    ctx->IASetInputLayout(input_layout.Get());

    ctx->VSSetShader(shader.Get(), nullptr, 0);
}

void VertexShader::draw_indexed(UINT count)
{
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    HANDLE_GFX_INFO(ctx->DrawIndexed(count, 0, 0));
}
