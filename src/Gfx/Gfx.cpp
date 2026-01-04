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

    depth_buffer.init(device.Get(), ctx.Get(), UINT(vp.Width), UINT(vp.Height), render_target_view.Get());

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

void Gfx::set_rasterizer_state(RasterizerState rs)
{
    if (current_rasterizer_state != rs)
    {
        auto p = reinterpret_cast<Microsoft::WRL::ComPtr<ID3D11RasterizerState> *>(&rasterizer_states);
        ctx->RSSetState(p[int(rs)].Get());
    }
}

Gfx::~Gfx()
{
}

void Gfx::clear(float *color)
{
    HANDLE_GFX_INFO(ctx->ClearRenderTargetView(render_target_view.Get(), color));

    HANDLE_GFX_INFO(
        ctx->ClearDepthStencilView(depth_buffer.get_view(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0));

    HANDLE_GFX_INFO(ctx->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_buffer.get_view()));
}

void Gfx::end_frame()
{
    HANDLE_GFX_ERR(swap_chain->Present(0, 0));
}

// TODO create alternate rasterizer state with no backface culling for 2-sided meshes
// {
// 	D3D11_RASTERIZER_DESC raster_desc = {};
//
// 	raster_desc.FillMode = D3D11_FILL_SOLID;
// 	raster_desc.CullMode = D3D11_CULL_NONE;
// 	raster_desc.FrontCounterClockwise = FALSE;
// 	raster_desc.DepthBias = 0;
// 	raster_desc.DepthBiasClamp = 0.0f;
// 	raster_desc.SlopeScaledDepthBias = 0.0f;
// 	raster_desc.DepthClipEnable = TRUE;
// 	raster_desc.ScissorEnable = FALSE;
// 	raster_desc.MultisampleEnable = FALSE;
// 	raster_desc.AntialiasedLineEnable = FALSE;
// }
