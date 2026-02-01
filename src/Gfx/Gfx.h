#pragma once

#include <DirectXMath.h>
#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <wrl.h>

class Gfx
{
    friend class GfxAccess;

  public:
    class DepthStencil
    {
      public:
        void init(ID3D11Device *device, ID3D11DeviceContext *ctx, UINT buffer_width, UINT buffer_height,
                  ID3D11RenderTargetView *render_target_view);

        Microsoft::WRL::ComPtr<ID3D11DepthStencilView> view;

        enum State : unsigned
        {
            DEPTH_BUFFER,
            ENVIRONMENT_BUFFER,

            TOTAL_STATE_COUNT_DO_NOT_USE
        };
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> states[State::TOTAL_STATE_COUNT_DO_NOT_USE];

        void bind_state(ID3D11DeviceContext *ctx, State state);
    };

    Gfx(HWND hwnd);
    ~Gfx();

    void clear(float *color);
    void end_frame();

    void set_view_proj_xform(DirectX::XMMATRIX mat);

    enum class RasterizerState
    {
        STANDARD,
        TWO_SIDED,
    };

    void set_rasterizer_state(RasterizerState rs);

    void bind_depth_stencil_state(DepthStencil::State state);

  private:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> ctx;

    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view;

    struct
    {
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> standard;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> two_sided;
    } rasterizer_states;
    RasterizerState current_rasterizer_state = RasterizerState::STANDARD;

    Microsoft::WRL::ComPtr<ID3D11BlendState> blend_state;

    D3D11_VIEWPORT viewport;
    DepthStencil depth_stencil;
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
