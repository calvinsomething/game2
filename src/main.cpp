#include <DirectXMath.h>
#include <windows.h>

#include <exception>

#include "Error.h"
#include "Global.h"
#include "Window.h"
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

struct InstanceData
{
    DirectX::XMMATRIX transform;
    uint32_t bone_start;
};

float bg_color[] = {0.15f, 0.15f, 0.2f, 1.0f};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Allocator<int>::allocator.init(50'000'000, 2000);

    Window window;

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

        StdVector<TextureVertex> *vertices = new StdVector<TextureVertex>;
        vertices->reserve(4096);

        StdVector<Vertex> *vertices2 = new StdVector<Vertex>;
        vertices2->reserve(4096);

        StdVector<uint32_t> *indices = new StdVector<uint32_t>;
        indices->reserve(4096);

        StdVector<uint32_t> *indices2 = new StdVector<uint32_t>;
        indices2->reserve(4096);

        StdVector<Material> *materials = new StdVector<Material>;
        materials->reserve(64);

        StdVector<Texture> textures;
        textures.reserve(64);

        Input input;
        input.register_devices();

        Gfx gfx(window.get_handle());

        Camera camera(gfx);

        ConstantBuffer lighting_buffer(gfx, ConstantBuffer::Slot::LIGHTING_BUFFER, &ConstantBuffer::bind_vs_and_ps,
                                       Global::lighting_data);
        // lighting_buffer.write(&Global::lighting_data, sizeof(Global::lighting_data));
        lighting_buffer.bind();

        Cube cube(gfx, L"assets/textures/minecraft_cube.dds", *vertices, *indices, *materials, textures);

        Model spider(gfx, "assets/models/spider/spider_clean.fbx", *vertices, *indices, *materials, textures);
        Model ninja(gfx, "assets/models/ninja.fbx", *vertices2, *indices2, *materials, textures);

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
                      DirectX::XMMatrixTranslation(0.0f, -2.0f, 0.0f));
        ninja.update(DirectX::XMMatrixScaling(0.6f, 0.6f, 0.6f) * DirectX::XMMatrixTranslation(0.0f, -1.5f, 0.0f));

        InstanceData instance_data[] = {{DirectX::XMMatrixTranspose(cube.get_transform()), 0},
                                        {DirectX::XMMatrixTranspose(spider.get_transform()), 0},
                                        {DirectX::XMMatrixTranspose(ninja.get_transform()), bone_matrices_count}};

        InstanceBuffer instance_buffer(gfx, &instance_data);

        for (auto &m : ninja.get_meshes())
        {
            bone_matrices_count += m.bone_matrices.size();
        }

        StructuredBuffer material_buffer(gfx, 0, materials->size(), sizeof(Material));
        material_buffer.bind_ps();
        material_buffer.update(materials->data(), materials->size() * sizeof(Material), 0);

        StructuredBuffer bone_data_buffer(gfx, 0, bone_matrices_count, sizeof(DirectX::XMMATRIX));

        VertexBuffer vb(gfx, *vertices);
        IndexBuffer ib(gfx, *indices);
        VertexBuffer vb2(gfx, *vertices2);
        IndexBuffer ib2(gfx, *indices2);
        TextureVertexShader vs(gfx);
        TexturePixelShader ps(gfx);
        VertexShader vs2(gfx);
        PixelShader ps2(gfx);

        ps.set_textures(textures);

        // deallocate
        delete vertices;
        vertices = 0;

        delete vertices2;
        vertices2 = 0;

        delete indices;
        indices = 0;

        delete indices2;
        indices2 = 0;

        delete materials;
        materials = 0;
        //

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

            ps.bind();

            cube.get_meshes()[0].constant_buffer.bind();
            vs.draw_indexed_instanced(0, cube.get_index_count(), 0, 1);

            bone_data_buffer.bind();

            bone_data_buffer.start_batch_update();

            size_t animation_data_offset = 0;
            for (Mesh<TextureVertex> &m : spider.get_meshes())
            {
                size_t animation_data_size = m.bone_matrices.size() * sizeof(m.bone_matrices[0]);
                bone_data_buffer.update(m.bone_matrices.data(), animation_data_size, animation_data_offset);
                animation_data_offset += animation_data_size;
            }
            for (Mesh<Vertex> &m : ninja.get_meshes())
            {
                size_t animation_data_size = m.bone_matrices.size() * sizeof(m.bone_matrices[0]);
                bone_data_buffer.update(m.bone_matrices.data(), animation_data_size, animation_data_offset);
                animation_data_offset += animation_data_size;
            }

            bone_data_buffer.end_batch_update();

            // spider

            UINT prev_index = cube.get_meshes()[0].get_index_count(),
                 prev_vertex = cube.get_meshes()[0].get_vertex_count();

            for (Mesh<TextureVertex> &m : spider.get_meshes())
            {
                UINT n = m.get_index_count();

                m.constant_buffer.bind();

                gfx.set_rasterizer_state(m.is_two_sided() ? Gfx::RasterizerState::TWO_SIDED
                                                          : Gfx::RasterizerState::STANDARD);

                vs.draw_indexed_instanced(prev_index, n, 1, 1, prev_vertex);

                prev_index += n;
                prev_vertex += m.get_vertex_count();
            }

            gfx.set_rasterizer_state(Gfx::RasterizerState::STANDARD);

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

                m.constant_buffer.bind();

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
