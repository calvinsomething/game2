#include <DirectXMath.h>
#include <vector>
#include <windows.h>

#include <exception>

#include "Error.h"
#include "Window.h"
#include "game/Clock.h"
#include "gfx/Camera.h"
#include "gfx/Gfx.h"
#include "gfx/IndexBuffer.h"
#include "gfx/InstanceBuffer.h"
#include "gfx/PixelShader.h"
#include "gfx/StructuredBuffer.h"
#include "gfx/VertexBuffer.h"
#include "gfx/VertexShader.h"
#include "input/Input.h"
#include "models/Cube.h"
#include "util.h"

namespace Global
{
bool running = true;
DirectX::XMFLOAT3 position{};
DirectX::XMFLOAT3 twelve_oclock{0.0f, 0.0f, 1.0f};
DirectX::XMFLOAT3 nine_oclock{-1.0f, 0.0f, 0.0f};

Clock clock;
}; // namespace Global

struct InstanceData
{
    DirectX::XMMATRIX transform;
    uint32_t bone_start;
};

float bg_color[] = {0.5f, 0.2f, 0.2f, 1.0f};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Window window;

    std::vector<TextureVertex> vertices;
    vertices.reserve(4096);

    std::vector<Vertex> vertices2;
    vertices.reserve(4096);

    std::vector<uint32_t> indices;
    indices.reserve(4096);

    std::vector<uint32_t> indices2;
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

        Model spider(gfx, "assets/models/spider/spider_clean.fbx", vertices, indices);
        Model ninja(gfx, "assets/models/ninja.fbx", vertices2, indices2);

        cube.set_position(-7.0f, 0.0f, 5.0f);
        spider.set_position(7.0f, 0.0f, 5.0f);
        ninja.set_position(11.0f, 0.0f, 0.0f);

        uint32_t bone_matrices_count = 0;
        for (auto &m : spider.get_meshes())
        {
            bone_matrices_count += m.bone_matrices.size();
        }

        cube.update(DirectX::XMMatrixScaling(0.6f, 0.6f, 0.6f) *
                    DirectX::XMMatrixRotationRollPitchYaw(DirectX::XM_PI * -0.5f, 0.0f, 0.0f));
        spider.update(DirectX::XMMatrixScaling(0.6f, 0.6f, 0.6f) * DirectX::XMMatrixRotationY(DirectX::XM_PI) *
                      DirectX::XMMatrixTranslation(0.0f, -3.0f, 0.0f));
        ninja.update(DirectX::XMMatrixScaling(0.6f, 0.6f, 0.6f) * DirectX::XMMatrixTranslation(0.0f, -1.5f, 0.0f));

        InstanceData instance_data[] = {{DirectX::XMMatrixTranspose(cube.get_transform()), 0},
                                        {DirectX::XMMatrixTranspose(spider.get_transform()), 0},
                                        {DirectX::XMMatrixTranspose(ninja.get_transform()), bone_matrices_count}};

        InstanceBuffer instance_buffer(gfx, &instance_data, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC,
                                       D3D11_CPU_ACCESS_WRITE);

        for (auto &m : ninja.get_meshes())
        {
            bone_matrices_count += m.bone_matrices.size();
        }

        StructuredBuffer structured_buffer(gfx, bone_matrices_count, sizeof(DirectX::XMMATRIX));

        VertexBuffer vb(gfx, vertices);
        IndexBuffer ib(gfx, indices);
        VertexBuffer vb2(gfx, vertices2);
        IndexBuffer ib2(gfx, indices2);
        TextureVertexShader vs(gfx);
        TexturePixelShader ps(gfx);
        VertexShader vs2(gfx);
        PixelShader ps2(gfx);

        Buffer *v_buffers[] = {&vb, &instance_buffer};
        VertexBuffers vbs(gfx, v_buffers);

        v_buffers[0] = &vb2;
        VertexBuffers vbs2(gfx, v_buffers);

        input.set_control_handler([&](unsigned char c) {
            float step = Global::clock.speed_to_distance(50.0f);

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

        Global::clock.set_max_fps(60);
        Global::clock.start();

        size_t i = 0;

        while (Global::running)
        {
            Global::clock.start_frame();

            input.handle_input();

            window.handle_messages();

            ++i;
            if (i > 200)
            {
                i = 0;
                spider.start_animation();
                ninja.start_animation();
            }

            spider.update(DirectX::XMMatrixIdentity());
            ninja.update(DirectX::XMMatrixIdentity());

            gfx.clear(bg_color);

            ib.bind();
            vbs.bind();
            vs.bind();

            ps.bind(&cube.get_textures()[0]);
            vs.draw_indexed_instanced(0, cube.get_index_count(), 0, 1);

            structured_buffer.bind();

            size_t animation_data_offset = 0;
            for (Mesh<TextureVertex> &m : spider.get_meshes())
            {
                size_t animation_data_size = m.bone_matrices.size() * sizeof(m.bone_matrices[0]);
                structured_buffer.update(m.bone_matrices.data(), animation_data_size, animation_data_offset);
                animation_data_offset += animation_data_size;
            }
            for (Mesh<Vertex> &m : ninja.get_meshes())
            {
                size_t animation_data_size = m.bone_matrices.size() * sizeof(m.bone_matrices[0]);
                structured_buffer.update(m.bone_matrices.data(), animation_data_size, animation_data_offset);
                animation_data_offset += animation_data_size;
            }

            // spider
            UINT prev_index = cube.get_index_count(), prev_vertex = cube.get_vertex_count();

            for (Mesh<TextureVertex> &m : spider.get_meshes())
            {
                Texture *t = m.get_texture();

                if (t)
                {
                    ps.bind(t);
                }

                UINT n = m.get_index_count();

                vs.draw_indexed_instanced(prev_index, n, 1, 1, prev_vertex);

                prev_index += n;
                prev_vertex += m.get_vertex_count();
            }

            // ninja
            ps2.bind();
            vs2.bind();
            ib2.bind();
            vbs2.bind();

            prev_index = 0;
            prev_vertex = 0;
            for (Mesh<Vertex> &m : ninja.get_meshes())
            {
                UINT n = m.get_index_count();

                vs2.draw_indexed_instanced(prev_index, n, 2, 1, prev_vertex);

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
