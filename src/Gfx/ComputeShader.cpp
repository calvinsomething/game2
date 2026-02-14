#include "ComputeShader.h"

#include "../Error.h"

ComputeShader::ComputeShader(Gfx &gfx, ShaderSource source) : Shader(gfx)
{
    std::string byte_code = load(source);

    HANDLE_GFX_ERR(device->CreateComputeShader(byte_code.data(), byte_code.size(), nullptr, shader.GetAddressOf()));
}

void ComputeShader::bind()
{
    ctx->CSSetShader(shader.Get(), nullptr, 0);
}
