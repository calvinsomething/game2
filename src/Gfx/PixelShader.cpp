#include "PixelShader.h"

#include "../Error.h"
#include <iostream>

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

void TexturePixelShader::set_textures(StdVector<Texture> &textures)
{
    ID3D11SamplerState *samplers[100] = {};
    ID3D11ShaderResourceView *views[100] = {};

    size_t n = 0;

    for (Texture &t : textures)
    {
        samplers[n] = t.get_sampler_state();
        views[n] = t.get_view();
        ++n;
    }

    ctx->PSSetSamplers(0, n, &samplers[0]);
    ctx->PSSetShaderResources(1, n, &views[0]);
}

// TODO change to materials
// void TexturePixelShader::set_material(const Material &material)
//{
//    ID3D11SamplerState *samplers[4] = {};
//    ID3D11ShaderResourceView *views[4] = {};
//
//    size_t n = 0;
//
//    Texture *textures[] = {material.diffuse_texture, material.normal_texture};
//
//    for (Texture *t : textures)
//    {
//        if (t)
//        {
//            samplers[n] = t->get_sampler_state();
//            views[n] = t->get_view();
//            ++n;
//        }
//    }
//
//    ctx->PSSetSamplers(0, n, &samplers[0]);
//    ctx->PSSetShaderResources(2, n, &views[0]);
//}
