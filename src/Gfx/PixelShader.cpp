#include "PixelShader.h"

#include "../Error.h"

PixelShader::PixelShader(Gfx &gfx) : Shader(gfx)
{
    auto byte_code = load("ps.cso");

    HANDLE_GFX_ERR(device->CreatePixelShader(byte_code.data(), byte_code.size(), nullptr, &shader));

    // D3D11_INPUT_ELEMENT_DESC layout[] = {
    //     {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
    //     {"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, sizeof(Vec4), D3D11_INPUT_PER_VERTEX_DATA, 0},
    // };

    // HANDLE_GFX_ERR(device->CreateInputLayout(nullptr, 0, byte_code.data(), byte_code.size(), 0));
}

void PixelShader::bind()
{
    // ctx->IASetInputLayout(input_layout.Get());

    ctx->PSSetShader(shader.Get(), nullptr, 0);
}
