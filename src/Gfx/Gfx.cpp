#include "Gfx.h"

#include <dxgidebug.h>

#include "../Error.h"
#include "../Global.h"

Gfx::Gfx(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = Global::client_width;
    sd.BufferDesc.Height = Global::client_height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = Global::clock.get_max_fps();
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

    viewport.Width = Global::client_width;
    viewport.Height = Global::client_height;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;

    HANDLE_GFX_INFO(ctx->RSSetViewports(1, &viewport));

    depth_stencil.init(device.Get(), ctx.Get(), UINT(viewport.Width), UINT(viewport.Height), render_target_view.Get());

    // Blending requires sorted mesh drawing to avoid Z-buffer writes for translucent/transparent pixels.
    D3D11_BLEND_DESC bd = {};

    bd.AlphaToCoverageEnable = FALSE;
    bd.IndependentBlendEnable = FALSE;
    bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    bd.RenderTarget[0].BlendEnable = TRUE;
    bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    HANDLE_GFX_ERR(device->CreateBlendState(&bd, blend_state.GetAddressOf()));

    {
        // create rasterizer states
        D3D11_RASTERIZER_DESC rd = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        HANDLE_GFX_ERR(device->CreateRasterizerState(&rd, rasterizer_states.standard.GetAddressOf()));

        rd.CullMode = D3D11_CULL_NONE;
        HANDLE_GFX_ERR(device->CreateRasterizerState(&rd, rasterizer_states.two_sided.GetAddressOf()));

        set_rasterizer_state(RasterizerState::STANDARD);
    }

    HANDLE_GFX_INFO(ctx->OMSetBlendState(blend_state.Get(), nullptr, 0xFFFFFFFF));
}

Gfx::~Gfx()
{
}

void Gfx::set_rasterizer_state(RasterizerState rs)
{
    if (current_rasterizer_state != rs)
    {
        auto p = reinterpret_cast<Microsoft::WRL::ComPtr<ID3D11RasterizerState> *>(&rasterizer_states);
        ctx->RSSetState(p[int(rs)].Get());

        current_rasterizer_state = rs;
    }
}

void Gfx::bind_depth_stencil_state(DepthStencil::State state)
{
    depth_stencil.bind_state(ctx.Get(), state);
}

void Gfx::clear(float *color)
{
    HANDLE_GFX_INFO(ctx->ClearRenderTargetView(render_target_view.Get(), color));

    HANDLE_GFX_INFO(
        ctx->ClearDepthStencilView(depth_stencil.view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0));

    HANDLE_GFX_INFO(ctx->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil.view.Get()));
}

void Gfx::end_frame()
{
    HANDLE_GFX_ERR(swap_chain->Present(0, 0));
}
