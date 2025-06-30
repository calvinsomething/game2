#include <DirectXMath.h>
#include <windows.h>

#include <exception>

#include "Error.h"
#include "Gfx/Gfx.h"
#include "Gfx/PixelShader.h"
#include "Gfx/VertexBuffer.h"
#include "Gfx/VertexShader.h"
#include "Window.h"

namespace Global
{
bool running = true;
};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Window window;

    try
    {
        Gfx gfx(window.get_handle());

        float color[] = {0.5f, 0.2f, 0.2f, 1.0f};
        uint32_t indices[] = {0, 1, 2};
        Vertex vertices[] = {{DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
                             {DirectX::XMFLOAT4(0.5f, -0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
                             {DirectX::XMFLOAT4(-0.5f, -0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}};

        IndexBuffer ib(gfx, indices);
        VertexBuffer vb(gfx, vertices);
        VertexShader vs(gfx);
        PixelShader ps(gfx);

        while (Global::running)
        {
            window.handle_message();

            gfx.clear(color);

            vs.bind();
            vb.bind();
            ib.bind();
            ps.bind();

            gfx.end_frame();
        }

        return 0;
    }
    catch (Error &exc)
    {
        MessageBoxA(window.get_handle(), exc.what(), exc.title(), MB_OK);
    }
    catch (std::exception &exc)
    {
        MessageBoxA(window.get_handle(), exc.what(), "Unknown Error", MB_OK);
    }
    catch (...)
    {
        MessageBoxA(window.get_handle(), "Something went wrong.", "Unknown Error", MB_OK);
    }

    return -1;
}
