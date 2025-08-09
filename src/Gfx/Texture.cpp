#include "Texture.h"

#include <Microsoft/DDSTextureLoader11.h>

#include "../Error.h"

void Texture::load()
{
    HANDLE_GFX_ERR(DirectX::CreateDDSTextureFromFile(device, ctx, L"assets/textures/minecraft_cube.dds",
                                                     reinterpret_cast<ID3D11Resource **>(texture.GetAddressOf()),
                                                     view.GetAddressOf()));
}

Texture::Texture(Gfx &gfx) : GfxAccess(gfx)
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

    load();
}

ID3D11SamplerState *Texture::get_sampler()
{
    return sampler_state.Get();
}

ID3D11ShaderResourceView *Texture::get_view()
{
    return view.Get();
}
