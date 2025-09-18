#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

class Gfx
{
    friend class GfxAccess;

    class DepthBuffer
    {
      public:
        void init(ID3D11Device *device, ID3D11DeviceContext *ctx, UINT buffer_width, UINT buffer_height,
                  ID3D11RenderTargetView *render_target_view);

        ID3D11DepthStencilView *get_view();

      private:
        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> view;
    };

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
    Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;

    DepthBuffer depth_buffer;
};

class GfxAccess
{
  protected:
    GfxAccess(Gfx &gfx) : device(gfx.device.Get()), ctx(gfx.ctx.Get())
    {
    }

    ID3D11Device *device;
    ID3D11DeviceContext *ctx;
};
