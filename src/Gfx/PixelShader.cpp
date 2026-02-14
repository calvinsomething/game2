#include "PixelShader.h"

#include "../Error.h"
#include "ComputeShader.h"

PixelShader::PixelShader(Gfx &gfx) : Shader(gfx)
{
    auto byte_code = load(ShaderSource::PIXEL);

    HANDLE_GFX_ERR(device->CreatePixelShader(byte_code.data(), byte_code.size(), nullptr, shader.GetAddressOf()));
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
    auto byte_code = load(ShaderSource::PIXEL_TEXTURE);

    HANDLE_GFX_ERR(device->CreatePixelShader(byte_code.data(), byte_code.size(), nullptr, shader.GetAddressOf()));
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
    auto byte_code = load(ShaderSource::PIXEL_SKYBOX);

    HANDLE_GFX_ERR(device->CreatePixelShader(byte_code.data(), byte_code.size(), nullptr, shader.GetAddressOf()));
}

void SkyboxPixelShader::load_texture(ComputeShader &cs, size_t cube_span)
{
    texture_array.initialize_texture_cube(cube_span);

    // TODO why is the UAV not loading into the CS properly
    D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
    uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
    uav_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    uav_desc.Texture2DArray.ArraySize = 6;
    uav_desc.Texture2DArray.FirstArraySlice = 0;
    uav_desc.Texture2DArray.MipSlice = 0;

    Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav;

    HANDLE_GFX_ERR(device->CreateUnorderedAccessView(texture_array.get_resource(), &uav_desc, uav.GetAddressOf()));

    HANDLE_GFX_INFO(
        ctx->CSSetUnorderedAccessViews(ComputeShader::Slot::OUTPUT_TEXTURE_2D, 1, uav.GetAddressOf(), nullptr));

    HANDLE_GFX_INFO(ctx->Dispatch(128, 128, 6));

    // unbind UAV so SRV can be bound
    ID3D11UnorderedAccessView *unbind = nullptr;
    HANDLE_GFX_INFO(ctx->CSSetUnorderedAccessViews(ComputeShader::Slot::OUTPUT_TEXTURE_2D, 1, &unbind, nullptr));

    // ID3D11ShaderResourceView *view = texture_array.get_view();

    // HANDLE_GFX_INFO(ctx->CSGetShaderResources(ComputeShader::Slot::OUTPUT_TEXTURE_2D, 1, &view));
}

void SkyboxPixelShader::bind()
{
    PixelShader::bind();

    ID3D11SamplerState *ss = texture_array.get_sampler_state();
    ID3D11ShaderResourceView *view = texture_array.get_view();

    ctx->PSSetSamplers(0, 1, &ss);
    ctx->PSSetShaderResources(0, 1, &view);
}
