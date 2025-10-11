#include "Gfx.h"

#include "../Error.h"

void Gfx::DepthBuffer::init(ID3D11Device *device, ID3D11DeviceContext *ctx, UINT buffer_width, UINT buffer_height,
                            ID3D11RenderTargetView *render_target_view)
{
    D3D11_TEXTURE2D_DESC texture_desc = {};
    texture_desc.Width = buffer_width;
    texture_desc.Height = buffer_height;
    texture_desc.MipLevels = 1;
    texture_desc.ArraySize = 1;
    texture_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    texture_desc.SampleDesc.Count = 1;
    texture_desc.SampleDesc.Quality = 0;
    texture_desc.Usage = D3D11_USAGE_DEFAULT;
    texture_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    texture_desc.CPUAccessFlags = 0;
    texture_desc.MiscFlags = 0;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;

    HANDLE_GFX_ERR(device->CreateTexture2D(&texture_desc, nullptr, texture.GetAddressOf()));

    D3D11_DEPTH_STENCIL_DESC stencil_desc = {};

    // Depth test parameters
    stencil_desc.DepthEnable = true;
    stencil_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    stencil_desc.DepthFunc = D3D11_COMPARISON_LESS;

    // Stencil test parameters
    // stencil_desc.StencilEnable = true;
    // stencil_desc.StencilReadMask = 0xFF;
    // stencil_desc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    // stencil_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    // stencil_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    // stencil_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    // stencil_desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    // stencil_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    // stencil_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    // stencil_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    // stencil_desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    Microsoft::WRL::ComPtr<ID3D11DepthStencilState> state;
    HANDLE_GFX_ERR(device->CreateDepthStencilState(&stencil_desc, state.GetAddressOf()));

    HANDLE_GFX_INFO(ctx->OMSetDepthStencilState(state.Get(), 0));

    // D3D11_DEPTH_STENCIL_VIEW_DESC view_desc;
    // view_desc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
    // view_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    // view_desc.Texture2D.MipSlice = 0;

    HANDLE_GFX_ERR(device->CreateDepthStencilView(texture.Get(),
                                                  nullptr, // Depth stencil desc
                                                  view.GetAddressOf()));

    // Bind the depth stencil view
    ctx->OMSetRenderTargets(1,                   // One rendertarget view
                            &render_target_view, // Render target view, created earlier
                            view.Get());         // Depth stencil view for the render target
}

ID3D11DepthStencilView *Gfx::DepthBuffer::get_view()
{
    return view.Get();
}
