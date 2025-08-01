#include "Gfx.h"

#include <dxgidebug.h>

#include "../Error.h"

Gfx::Gfx(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 640;
    sd.BufferDesc.Height = 640;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    UINT createDeviceFlags =
#ifdef NDEBUG
        0;
#else
        D3D11_CREATE_DEVICE_DEBUG;

    GfxError::init_dxgi_debug();

#endif

    D3D_FEATURE_LEVEL feature_level_options = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL feature_level;

    HANDLE_GFX_ERR(D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, &feature_level_options,
        1, // feature level options and number of options
        D3D11_SDK_VERSION, &sd, swap_chain.GetAddressOf(), device.GetAddressOf(), &feature_level, ctx.GetAddressOf()));

#ifndef NDEBUG
    GfxError::init_d3d11_debug(device.Get());

#endif

    Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;

    HANDLE_GFX_ERR(
        swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(back_buffer.GetAddressOf())));

    HANDLE_GFX_ERR(device->CreateRenderTargetView(back_buffer.Get(), nullptr, render_target_view.GetAddressOf()));

    D3D11_VIEWPORT vp;
    vp.Width = 640;
    vp.Height = 640;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    HANDLE_GFX_INFO(ctx->RSSetViewports(1, &vp));
}

Gfx::~Gfx()
{
}

void Gfx::clear(float *color)
{
    HANDLE_GFX_INFO(ctx->ClearRenderTargetView(render_target_view.Get(), color));

    HANDLE_GFX_INFO(ctx->OMSetRenderTargets(1, render_target_view.GetAddressOf(), nullptr));
}

void Gfx::end_frame()
{
    HANDLE_GFX_ERR(swap_chain->Present(0, 0));
}
