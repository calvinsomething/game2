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

    std::vector<TextureVertex> vertices;
    vertices.reserve(4096);

    std::vector<uint32_t> indices;
    indices.reserve(4096);

    try
    {

#ifndef NDEBUG
        if (!AllocConsole())
        {
            throw WindowsError(__FILE__, __LINE__);
        }

        {
            FILE *f = 0;
            freopen_s(&f, "CONOUT$", "w", stdout);
        }
#endif
        Input input;
        input.register_devices();

        Gfx gfx(window.get_handle());

        Camera camera(gfx);

        Cube cube(vertices, indices);
        cube.load_texture(gfx, L"assets/textures/minecraft_cube.dds");

        Model cat(gfx, "assets/models/spider/spider_clean.fbx", vertices, indices);

        cube.set_position(-7.0f, 0.0f, 5.0f);
        cat.set_position(7.0f, 0.0f, 5.0f);
        cat.update(DirectX::XMMatrixScaling(1.1f, 1.1f, 1.1f));

        DirectX::XMMATRIX xforms[] = {DirectX::XMMatrixIdentity(), DirectX::XMMatrixIdentity()};
        // {,
        //                               };

        float bg_color[] = {0.5f, 0.2f, 0.2f, 1.0f};

        InstanceBuffer instance_buffer(gfx, &xforms, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC,
                                       D3D11_CPU_ACCESS_WRITE);

        cube.update(DirectX::XMMatrixRotationRollPitchYaw(DirectX::XM_PI * -0.5f, 0.0f, 0.0f));
        cat.update(DirectX::XMMatrixRotationRollPitchYaw(DirectX::XM_PI * -0.5f, DirectX::XM_PI, 0.0f) *
                   DirectX::XMMatrixTranslation(0.0f, -4.0f, 0.0f));

        DirectX::XMMATRIX xf[] = {DirectX::XMMatrixTranspose(cube.get_transform()),
                                  DirectX::XMMatrixTranspose(cat.get_transform())};

        instance_buffer.update(xf, sizeof(xf));

        VertexBuffer vb(gfx, vertices);
        IndexBuffer ib(gfx, indices);
        // Texture cube_tex(gfx, L"assets/textures/minecraft_cube.dds"), cat_tex(gfx, cat.get_ai_texture());
        TextureVertexShader vs(gfx);
        TexturePixelShader ps(gfx);
        // VertexShader vs(gfx);
        // PixelShader ps(gfx);

        ib.bind();
        vs.bind();

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

            ps.bind(&cube.get_textures()[0]);
            vs.draw_indexed_instanced(0, cube.get_index_count(), 0, 1);

            UINT prev_index = cube.get_index_count(), prev_vertex = cube.get_vertex_count();

            for (Mesh<TextureVertex> &m : cat.get_meshes())
            {

                auto t = m.get_texture();

                if (t)
                {
                    ps.bind(t);
                }

                UINT n = m.get_index_count();

                vs.draw_indexed_instanced(prev_index, n, 1, 1, prev_vertex);

                prev_index += n;
                prev_vertex += m.get_vertex_count();
            }

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
