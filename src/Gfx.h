#include <Windows.h>
#include <d3d11.h>

#include <stdexcept>

class Gfx
{
  public:
    Gfx(HWND hwnd)
    {
        DXGI_SWAP_CHAIN_DESC sd{};
        sd.BufferCount = 1;
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
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

        UINT createDeviceFlags =
#ifdef NDEBUG
            0;
#else
            D3D11_CREATE_DEVICE_DEBUG;
#endif

        HRESULT hr = S_OK;

        D3D_FEATURE_LEVEL feature_level_options = D3D_FEATURE_LEVEL_11_0;
        D3D_FEATURE_LEVEL feature_level;

        hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
                                           &feature_level_options, 1, // feature level options and number of options
                                           D3D11_SDK_VERSION, &sd, &swap_chain, &device, &feature_level, &ctx);

        if (FAILED(hr))
        {
            throw std::runtime_error("failed to create device and swap chain");
        }

        ID3D11Texture2D *pBackBuffer;
        ID3D11RenderTargetView *render_target_view;

        hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID *)&pBackBuffer);
        if (FAILED(hr))
        {
            throw std::runtime_error("failed to create swap chain");
        }

        hr = device->CreateRenderTargetView(pBackBuffer, nullptr, &render_target_view);
        if (FAILED(hr))
        {
            throw std::runtime_error("failed to create render target view");
        }

        ctx->OMSetRenderTargets(1, &render_target_view, nullptr);

        D3D11_VIEWPORT vp;
        vp.Width = 640;
        vp.Height = 480;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;

        ctx->RSSetViewports(1, &vp);

        FLOAT color[] = {0.1f, 0.7f, 0.1f, 1.0f};
        ctx->ClearRenderTargetView(render_target_view, color);

        swap_chain->Present(0, 0);
        if (FAILED(hr))
        {
            throw std::runtime_error("failed to present");
        }
    }

  private:
    ID3D11DeviceContext *ctx = 0;
    IDXGISwapChain *swap_chain = 0;
    ID3D11Device *device = 0;
};
