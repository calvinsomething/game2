#include "VertexShader.h"

#include "../Error.h"

const DirectX::XMMATRIX identity_matrix(1, 0, 0, 0, //
                                        0, 1, 0, 0, //
                                        0, 0, 1, 0, //
                                        0, 0, 0, 1);

void bind_proc(ID3D11DeviceContext *ctx, ID3D11Buffer *buffer)
{
    ctx->VSSetConstantBuffers(0, 1, &buffer);

    // TODO use XMFLOAT4 in Vertex, load into XMMATRIX/VECTOR with XMLoadFloat4
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

void VertexShader::set_model_transform(DirectX::XMFLOAT3X3 mat)
{
    auto transform = DirectX::XMLoadFloat3x3(&mat);

    constant_buffer.write(&transform, sizeof(transform));
}

void VertexShader::bind()
{
    constant_buffer.bind();

    ctx->IASetInputLayout(input_layout.Get());

    ctx->VSSetShader(shader.Get(), nullptr, 0);
}
