#include "Texture.h"

#include <Microsoft/DDSTextureLoader11.h>
#include <stdexcept>

#include "../Error.h"

void Texture::load(const wchar_t *file_name)
{
    DirectX::DDS_ALPHA_MODE mode = DirectX::DDS_ALPHA_MODE_STRAIGHT;

    HANDLE_GFX_ERR(DirectX::CreateDDSTextureFromFile(device, ctx, file_name,
                                                     reinterpret_cast<ID3D11Resource **>(texture.GetAddressOf()),
                                                     view.GetAddressOf(), 0, &mode));
}

void Texture::load(const aiTexture *ai_texture)
{
    auto size = ai_texture->mHeight;

    if (!size)
    {
        throw std::runtime_error("Texture data is compressed.");

        const char *formats[] = {"jpg", "png"}, *format = 0;
        for (const char *f : formats)
        {
            if (ai_texture->CheckFormat(f))
            {
                format = f;
            }
        }

        // TODO
        // load file based on format
    }

    if (strcmp(ai_texture->achFormatHint, "argb8888"))
    {
        throw std::runtime_error("Texture format is not argb8888");
    }

    size *= ai_texture->mWidth;

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = ai_texture->mWidth;
    desc.Height = ai_texture->mHeight;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA init_data;
    init_data.pSysMem = ai_texture->pcData;
    init_data.SysMemPitch = ai_texture->mWidth;
    init_data.SysMemSlicePitch = 0;

    HANDLE_GFX_ERR(device->CreateTexture2D(&desc, &init_data, texture.GetAddressOf()));

    D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
    view_desc.Format = DXGI_FORMAT_UNKNOWN;
    view_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

    view_desc.Texture2D.MipLevels = 0;
    view_desc.Texture2D.MostDetailedMip = -1;

    HANDLE_GFX_ERR(device->CreateShaderResourceView(texture.Get(), &view_desc, view.GetAddressOf()));
}

Texture::Texture(Gfx &gfx, const wchar_t *file_name) : GfxAccess(gfx)
{
    D3D11_SAMPLER_DESC sampler_desc = {};
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    HANDLE_GFX_ERR(device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf()));

    load(file_name);
}

Texture::Texture(Gfx &gfx, const aiTexture *ai_texture) : GfxAccess(gfx)
{
    D3D11_SAMPLER_DESC sampler_desc = {};
    sampler_desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampler_desc.MinLOD = 0;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

    HANDLE_GFX_ERR(device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf()));
}

ID3D11SamplerState *Texture::get_sampler_state()
{
    return sampler_state.Get();
}

ID3D11ShaderResourceView *Texture::get_view()
{
    return view.Get();
}
