#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

class Gfx
{
    friend class Buffer;
    friend class Shader;

  public:
    Gfx(HWND hwnd);
    ~Gfx();

    void clear(float *color);
    void end_frame();

    void set_view_proj_xform(DirectX::XMMATRIX mat);

  private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> ctx;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;
};
