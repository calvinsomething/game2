#pragma once

#include "Gfx.h"

class Gfx::DepthStencil
{
  public:
    void init(ID3D11Device *device, ID3D11DeviceContext *ctx, UINT buffer_width, UINT buffer_height,
              ID3D11RenderTargetView *render_target_view, D3D11_DEPTH_STENCIL_DESC *stencil_desc = nullptr);

    ID3D11DepthStencilView *get_view();

  private:
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> view;
};
