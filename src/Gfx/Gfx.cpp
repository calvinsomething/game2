#include "Gfx.h"

#include "../Error.h"

#ifndef NDEBUG
namespace Global
{
ID3D11InfoQueue *info_queue;
};
#endif

Gfx::Gfx(HWND hwnd)
{
    DXGI_SWAP_CHAIN_DESC sd{};
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 640;
    sd.BufferDesc.Height = 480;
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
#endif

    D3D_FEATURE_LEVEL feature_level_options = D3D_FEATURE_LEVEL_11_0;
    D3D_FEATURE_LEVEL feature_level;

    HANDLE_WIN_ERR(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
                                                 &feature_level_options,
                                                 1, // feature level options and number of options
                                                 D3D11_SDK_VERSION, &sd, &swap_chain, &device, &feature_level, &ctx));

#ifndef NDEBUG
    // ID3D11Debug *debug;
    // HANDLE_WIN_ERR(device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void **>(&debug)));

    HANDLE_WIN_ERR(device->QueryInterface(__uuidof(ID3D11InfoQueue), reinterpret_cast<void **>(&Global::info_queue)));

    // Global::info_queue->PushEmptyStorageFilter();
    // Global::info_queue->PushEmptyRetrievalFilter();

    D3D11_MESSAGE_CATEGORY categories[] = {D3D11_MESSAGE_CATEGORY_APPLICATION_DEFINED,
                                           D3D11_MESSAGE_CATEGORY_MISCELLANEOUS,
                                           D3D11_MESSAGE_CATEGORY_INITIALIZATION,
                                           D3D11_MESSAGE_CATEGORY_CLEANUP,
                                           D3D11_MESSAGE_CATEGORY_COMPILATION,
                                           D3D11_MESSAGE_CATEGORY_STATE_CREATION,
                                           D3D11_MESSAGE_CATEGORY_STATE_SETTING,
                                           D3D11_MESSAGE_CATEGORY_STATE_GETTING,
                                           D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION,
                                           D3D11_MESSAGE_CATEGORY_EXECUTION,
                                           D3D11_MESSAGE_CATEGORY_SHADER};

    D3D11_MESSAGE_SEVERITY severities[] = {D3D11_MESSAGE_SEVERITY_CORRUPTION, D3D11_MESSAGE_SEVERITY_ERROR,
                                           D3D11_MESSAGE_SEVERITY_WARNING};

    D3D11_INFO_QUEUE_FILTER filter = {};
    filter.AllowList = {
        11,
        categories,
        3,
        severities,
    };

    Global::info_queue->AddStorageFilterEntries(&filter);
#endif

    ID3D11Texture2D *back_buffer;

    HANDLE_GFX_ERR(swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void **>(&back_buffer)));

    HANDLE_GFX_ERR(device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view));

    back_buffer->Release();

    D3D11_VIEWPORT vp;
    vp.Width = 640;
    vp.Height = 480;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;

    ctx->RSSetViewports(1, &vp);
}

Gfx::~Gfx()
{
#ifndef NDEBUG
    if (Global::info_queue)
    {
        Global::info_queue->Release();
    }
#endif
}

void Gfx::clear(float *color)
{
    ctx->ClearRenderTargetView(render_target_view, color);
}

void Gfx::end_frame()
{
    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    ctx->OMSetRenderTargets(1, &render_target_view, nullptr);

    ctx->DrawIndexed(3, 0, 0);

    HANDLE_GFX_ERR(swap_chain->Present(0, 0));
}
