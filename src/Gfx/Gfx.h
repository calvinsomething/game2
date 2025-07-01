#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

class Shader;

class Gfx
{
    friend class Buffer;
    friend class Shader;

  public:
    Gfx(HWND hwnd);
    ~Gfx();

    void clear(float *color);
    void end_frame();

  private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> ctx;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
};
