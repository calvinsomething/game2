#include "Texture.h"

#include <iostream>
#include <stdexcept>

#include <Microsoft/DDSTextureLoader11.h>

#include "../Error.h"
#include "../util.h"

// Texture
//
Microsoft::WRL::ComPtr<ID3D11SamplerState> Texture::sampler_state;

Texture::Texture(Gfx &gfx) : GfxAccess(gfx)
{
    if (!sampler_state.Get())
    {
        init_sampler_state();
    }
}

void Texture::init_sampler_state()
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

// Texture2D
//
Texture2D::Texture2D(Gfx &gfx) : Texture(gfx)
{
}

Texture2D::Texture2D(Gfx &gfx, const wchar_t *file_name) : Texture(gfx)
{
    load(file_name);
}

void Texture2D::load(const wchar_t *file_name)
{
    DirectX::DDS_ALPHA_MODE mode = DirectX::DDS_ALPHA_MODE_STRAIGHT;

    HANDLE_GFX_ERR(DirectX::CreateDDSTextureFromFile(device, ctx, file_name,
                                                     reinterpret_cast<ID3D11Resource **>(texture.GetAddressOf()),
                                                     view.GetAddressOf(), 0, &mode));
}

void Texture2D::load(const aiTexture *ai_texture)
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

    view_desc.Texture2D.MipLevels = 1;
    view_desc.Texture2D.MostDetailedMip = -1;

    HANDLE_GFX_ERR(device->CreateShaderResourceView(texture.Get(), &view_desc, view.GetAddressOf()));
}

// TextureCube
//
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_HDR
#include <nothings/stb/stb_image.h>

void Texture2D::load_cube_from_hdr(const char *file_name)
{
    int x, y, n;
    unsigned char *data = stbi_load(file_name, &x, &y, &n, 4);

    std::cout << "HDR file contains " << n << " channels\n";

    if (!data)
    {
        const char *reason = stbi_failure_reason();
        if (reason)
        {
            auto msg = build_string("Failed to load file '", file_name, "': ", reason);
            throw std::runtime_error(msg);
        }

        auto msg = build_string("Failed to load file: ", file_name);
        throw std::runtime_error(msg);
    }

    D3D11_TEXTURE2D_DESC texture_desc = {};
    texture_desc.Width = x / 2;
    texture_desc.Height = y;
    texture_desc.ArraySize = 6;
    texture_desc.MipLevels = 1;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

    UINT pitch = x * 2; //* 4;
    UINT size = pitch * y;

    unsigned char *buffer = static_cast<unsigned char *>(malloc(size));
    memcpy(buffer, data, size);

    // TODO why does this crash
    // stbi_image_free(data);

    D3D11_SUBRESOURCE_DATA init_data[6];

    for (auto &id : init_data)
    {
        id.pSysMem = buffer;
        id.SysMemPitch = pitch;
        id.SysMemSlicePitch = 0;
    }

    HANDLE_GFX_ERR(device->CreateTexture2D(&texture_desc, init_data, texture.GetAddressOf()));

    D3D11_SHADER_RESOURCE_VIEW_DESC view_desc = {};
    view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;

    view_desc.TextureCube = {};
    view_desc.TextureCube.MipLevels = 1;

    HANDLE_GFX_ERR(device->CreateShaderResourceView(texture.Get(), &view_desc, view.GetAddressOf()));
}
