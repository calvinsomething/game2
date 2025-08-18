#include <DirectXMath.h>
#include <vector>
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
#include "input/Input.h"
#include "models/Cube.h"
#include "util.h"

namespace Global
{
bool running = true;
DirectX::XMFLOAT3 position{};
DirectX::XMFLOAT3 twelve_oclock{0.0f, 0.0f, 1.0f};
DirectX::XMFLOAT3 nine_oclock{-1.0f, 0.0f, 0.0f};
}; // namespace Global

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Window window;

    std::vector<Vertex> vertices;
    vertices.reserve(4096);

    std::vector<uint32_t> indices;
    indices.reserve(4096);

    try
    {
        Input input;
        input.register_devices();

        Gfx gfx(window.get_handle());

        Camera camera(gfx);

        Cube cube(vertices, indices);

        Model cat("assets/models/cat/cat.obj", vertices, indices);

        cube.set_position(-5.0f, 0.0f, 0.0f);
        cat.set_position(5.0f, 0.0f, 0.0f);
        cat.update(DirectX::XMMatrixScaling(0.3f, 0.3f, 0.3f));

        DirectX::XMMATRIX xforms[] = {DirectX::XMMatrixRotationRollPitchYaw(0.005f, 0.002f, 0.003f),
                                      DirectX::XMMatrixRotationRollPitchYaw(-0.005f, -0.002f, -0.003f)};

        float bg_color[] = {0.5f, 0.2f, 0.2f, 1.0f};

        InstanceBuffer instance_buffer(gfx, &xforms, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC,
                                       D3D11_CPU_ACCESS_WRITE);

        VertexBuffer vb(gfx, vertices);
        IndexBuffer ib(gfx, indices);
        // Texture texture(gfx);
        // TextureVertexShader vs(gfx);
        // TexturePixelShader ps(gfx, texture);
        VertexShader vs(gfx);
        PixelShader ps(gfx);

        ib.bind();
        vs.bind();
        ps.bind();

        Buffer *v_buffers[] = {&vb, &instance_buffer};
        VertexBuffers vbs(gfx, v_buffers);

        vbs.bind();

        input.set_control_handler([&](unsigned char c) {
            constexpr float step = 1.0f;

            switch (c)
            {
            case 'W':
                Global::position = Global::position + Global::twelve_oclock * step;
                break;
            case 'A':
                Global::position = Global::position + Global::nine_oclock * step;
                break;
            case 'S':
                Global::position = Global::position - Global::twelve_oclock * step;
                break;
            case 'D':
                Global::position = Global::position - Global::nine_oclock * step;
                break;
            case 'Q':
                camera.increase_azimuth(0.2f);
                break;
            case 'E':
                camera.increase_azimuth(-0.2f);
                break;
            }
        });

        while (Global::running)
        {
            input.handle_input();

            window.handle_messages();

            gfx.clear(bg_color);

            cube.update(xforms[0]);
            cat.update(xforms[1]);

            DirectX::XMMATRIX xf[] = {DirectX::XMMatrixTranspose(cube.get_transform()),
                                      DirectX::XMMatrixTranspose(cat.get_transform())};

            instance_buffer.update(xf, sizeof(xf));

            vs.draw_indexed_instanced(0, cube.get_index_count(), 0, 1);
            vs.draw_indexed_instanced(cube.get_index_count(), cat.get_index_count(), 1, 1, cube.get_vertex_count());

            camera.update(Global::position);

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
