#include <Windows.h>
#include <d3d11.h>

class Gfx
{
  public:
    Gfx(HWND hwnd);

  private:
    ID3D11DeviceContext *ctx = 0;
    IDXGISwapChain *swap_chain = 0;
    ID3D11Device *device = 0;
};
