#include "Gfx.h"

#include "../Error.h"
#include "../Global.h"

constexpr UINT width = 4092, height = 4092;

void Gfx::ShadowMap::init(Gfx &gfx, UINT width, UINT height)
{
    D3D11_TEXTURE2D_DESC texture_desc = {};
    texture_desc.Width = width;
    texture_desc.Height = height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_R32_TYPELESS;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

    HANDLE_GFX_ERR(gfx.device->CreateTexture2D(&texture_desc, nullptr, texture.GetAddressOf()));

    D3D11_DEPTH_STENCIL_DESC depth_state_desc = {};
    depth_state_desc.DepthEnable = true;
    depth_state_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depth_state_desc.DepthFunc = D3D11_COMPARISON_LESS;

    HANDLE_GFX_ERR(gfx.device->CreateDepthStencilState(&depth_state_desc, depth_state.GetAddressOf()));

    D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc = {};
    dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
    dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsv_desc.Flags = 0;
    dsv_desc.Texture2D.MipSlice = 0;

    HANDLE_GFX_ERR(gfx.device->CreateDepthStencilView(texture.Get(), &dsv_desc, depth_view.GetAddressOf()));

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
    srv_desc.Format = DXGI_FORMAT_R32_FLOAT;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;

    HANDLE_GFX_ERR(gfx.device->CreateShaderResourceView(texture.Get(), &srv_desc, srv.GetAddressOf()));

    D3D11_SAMPLER_DESC sampler_desc = {};
    sampler_desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    sampler_desc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampler_desc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampler_desc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
    sampler_desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
    sampler_desc.MinLOD = 0.0f;
    sampler_desc.MaxLOD = D3D11_FLOAT32_MAX;

    HANDLE_GFX_ERR(gfx.device->CreateSamplerState(&sampler_desc, sampler_state.GetAddressOf()));

    viewport.Width = width;
    viewport.Height = height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
}
