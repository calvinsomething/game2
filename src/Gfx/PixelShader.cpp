#include "PixelShader.h"

#include "../Error.h"

PixelShader::PixelShader(Gfx &gfx) : Shader(gfx)
{
    auto byte_code = load("ps.cso");

    HANDLE_GFX_ERR(device->CreatePixelShader(byte_code.data(), byte_code.size(), nullptr, &shader));
}

void PixelShader::bind()
{
    ctx->PSSetShader(shader.Get(), nullptr, 0);
}

// TODO
// void PixelShader::set_materials(const Material &material)
// {
// ctx->PSSetShaderResources(0, n, &views[0]);
// }

// TexturePixelShader
TexturePixelShader::TexturePixelShader(Gfx &gfx) : PixelShader(gfx)
{
    auto byte_code = load("ps_tex.cso");

    HANDLE_GFX_ERR(device->CreatePixelShader(byte_code.data(), byte_code.size(), nullptr, &shader));
}

void TexturePixelShader::set_textures(StdVector<Texture2D> &textures)
{
    ID3D11SamplerState *samplers[100] = {};
    ID3D11ShaderResourceView *views[100] = {};

    size_t n = 0;

    for (Texture2D &t : textures)
    {
        samplers[n] = t.get_sampler_state();
        views[n] = t.get_view();
        ++n;
    }

    ctx->PSSetSamplers(0, n, &samplers[0]);
    ctx->PSSetShaderResources(2, n, &views[0]);
}

// TODO change to materials instead of textures
// void TexturePixelShader::set_material(const Material &material)
//{
//}

SkyboxPixelShader::SkyboxPixelShader(Gfx &gfx) : PixelShader(gfx), texture_array(gfx)
{
    auto byte_code = load("ps_skybox.cso");

    HANDLE_GFX_ERR(device->CreatePixelShader(byte_code.data(), byte_code.size(), nullptr, &shader));
}

void SkyboxPixelShader::load_texture(const char *file_name)
{
    texture_array.load_cube_from_hdr(file_name);
}

void SkyboxPixelShader::bind()
{
    PixelShader::bind();

    ID3D11SamplerState *ss = texture_array.get_sampler_state();
    ID3D11ShaderResourceView *view = texture_array.get_view();

    ctx->PSSetSamplers(0, 1, &ss);
    ctx->PSSetShaderResources(0, 1, &view);
}
