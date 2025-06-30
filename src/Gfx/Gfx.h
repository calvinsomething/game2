#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <dxgi.h>

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
    ID3D11Device *device = 0;
    IDXGISwapChain *swap_chain = 0;
    ID3D11DeviceContext *ctx = 0;

    ID3D11RenderTargetView *render_target_view;
};
