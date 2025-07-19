#include <DirectXMath.h>
#include <windows.h>

#include <exception>

#include "Error.h"
#include "Gfx/Camera.h"
#include "Gfx/Gfx.h"
#include "Gfx/IndexBuffer.h"
#include "Gfx/InstanceBuffer.h"
#include "Gfx/PixelShader.h"
#include "Gfx/VertexBuffer.h"
#include "Gfx/VertexShader.h"
#include "Window.h"
#include "models/Cube.h"

namespace Global
{
bool running = true;
};

Vertex vertices[] = {{DirectX::XMFLOAT4(-5.0f, 5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
                     {DirectX::XMFLOAT4(5.0f, 5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
                     {DirectX::XMFLOAT4(5.0f, -5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
                     {DirectX::XMFLOAT4(-5.0f, -5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
                     {DirectX::XMFLOAT4(-5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)},
                     {DirectX::XMFLOAT4(5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)},
                     {DirectX::XMFLOAT4(5.0f, -5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
                     {DirectX::XMFLOAT4(-5.0f, -5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)}};

uint32_t indices[] = {0, 1, 2, 2, 3, 0, 0, 4, 5, 5, 1, 0, 4, 7, 6, 6, 5, 4,
                      4, 0, 3, 3, 7, 4, 3, 2, 6, 6, 7, 3, 1, 5, 6, 6, 2, 1};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Window window;

    try
    {
        Gfx gfx(window.get_handle());

        Camera camera(gfx);

        Cube cube(gfx);

        float color[] = {0.5f, 0.2f, 0.2f, 1.0f};

        InstanceBuffer instance_buffer(gfx, cube.get_transforms(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC,
                                       D3D11_CPU_ACCESS_WRITE);

        VertexBuffer vb(gfx, vertices);
        IndexBuffer ib(gfx, indices);
        VertexShader vs(gfx);
        PixelShader ps(gfx);

        ib.bind();
        vs.bind();
        ps.bind();

        Buffer *v_buffers[] = {&vb, &instance_buffer};
        VertexBuffers vbs(gfx, v_buffers);

        vbs.bind();

        while (Global::running)
        {
            window.handle_messages();

            gfx.clear(color);

            cube.bind();

            cube.update();

            instance_buffer.update(cube.get_transforms(), sizeof(Cube::Transforms));

            vs.draw_indexed(ARRAYSIZE(indices));

            camera.update();

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
