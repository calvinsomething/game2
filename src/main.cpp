#include <DirectXMath.h>
#include <windows.h>

#include <exception>

#include "Error.h"
#include "Global.h"
#include "Window.h"
#include "game/Camera.h"
#include "game/Character.h"
#include "game/Controller.h"
#include "game/Skybox.h"
#include "gfx/Gfx.h"
#include "gfx/IndexBuffer.h"
#include "gfx/InstanceBuffer.h"
#include "gfx/PixelShader.h"
#include "gfx/StructuredBuffer.h"
#include "gfx/VertexBuffer.h"
#include "gfx/VertexShader.h"
#include "input/Input.h"
#include "models/Cube.h"
#include "models/Model.h"
#include "util.h"

float bg_color[] = {0.15f, 0.15f, 0.2f, 1.0f};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Allocator<int>::allocator.init(500'000'000, 10'000);

    Window window;

    RECT rect = window.get_rect();
    Global::client_width = rect.right - rect.left;
    Global::client_height = rect.bottom - rect.top;

    window.set_full_screen();

    Global::clock.set_max_fps(60);
    Global::movement_speed = Global::clock.speed_to_distance(30.0f);

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

        StdVector<Texture2D> textures;
        textures.reserve(64);

        Global::input.register_devices();

        Gfx gfx(window.get_handle());

        Camera camera(gfx);

        Controller controller;

        ConstantBuffer lighting_buffer(gfx, ConstantBuffer::Slot::LIGHTING_BUFFER, &ConstantBuffer::bind_vs_and_ps,
                                       Global::lighting_data); // bound as static/read-only
        lighting_buffer.bind();

        InstanceData instance_data[4]{};

        Cube cube(gfx, L"assets/textures/minecraft_cube.dds", *vertices, *indices, *materials, textures,
                  instance_data[0]);
        cube.set_correction_transform(DirectX::XMMatrixMultiply(
            DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{std::sqrt(0.5f), 0.0f, 0.0f, -std::sqrt(0.5f)}),
            DirectX::XMMatrixTranslation(0.0f, 5.0f, 0.0f)));

        Model spider(gfx, "assets/models/spider/spider_clean.fbx", *vertices, *indices, *materials, textures,
                     instance_data[1]);
        spider.set_correction_transform(
            DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(0.6f, 0.6f, 0.6f),
                                      DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{0.0f, 1.0f, 0.0f, 0.0f})),
            0.0f, *vertices);

        Model ninja(gfx, "assets/models/ninja.fbx", *vertices2, *indices2, *materials, textures, instance_data[2],
                    spider.get_bone_count());
        ninja.set_correction_transform(
            DirectX::XMMatrixMultiply(DirectX::XMMatrixScaling(0.6f, 0.6f, 0.6f),
                                      DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{0.0f, 1.0f, 0.0f, 0.0f})),
            0.0f, *vertices2);

        Model grass_platform(gfx, "assets/models/GrassPlatform/grass_platform.obj", *vertices2, *indices2, *materials,
                             textures, instance_data[3]);
        grass_platform.set_correction_transform(
            DirectX::XMMatrixMultiply(
                DirectX::XMMatrixScaling(70.0f, 70.0f, 70.0f),
                DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{std::sqrt(0.5f), 0.0f, 0.0f, -std::sqrt(0.5f)})),
            1.0f, *vertices2);

        Character player_character(ninja);
        player_character.set_position({0.0f, 0.0f, -10.0f});

        cube.set_position({-7.0f, 0.0f, 5.0f});
        spider.set_position({7.0f, 0.0f, 5.0f});

        grass_platform.update();
        cube.update();
        spider.update();
        ninja.update();

        InstanceBuffer instance_buffer(gfx, &instance_data);

        StructuredBuffer material_buffer(gfx, 1, materials->size(), sizeof(Material));
        material_buffer.bind_ps();
        material_buffer.update(materials->data(), materials->size() * sizeof(Material), 0);

        StructuredBuffer bone_data_buffer(gfx, 0, spider.get_bone_count() + ninja.get_bone_count(),
                                          sizeof(DirectX::XMMATRIX));

        VertexBuffer vb(gfx, *vertices);
        IndexBuffer ib(gfx, *indices);
        VertexBuffer vb2(gfx, *vertices2);
        IndexBuffer ib2(gfx, *indices2);
        TextureVertexShader vs(gfx);
        TexturePixelShader ps(gfx);
        VertexShader vs2(gfx);
        PixelShader ps2(gfx);

        // Skybox
        Skybox skybox(gfx, "assets/hdr/DayInTheClouds4k.hdr");

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

        Global::clock.start();

        while (Global::running)
        {
            Global::clock.start_frame();

            window.handle_messages();

            Global::input.handle_input();

            camera.update(controller.get_camera_controls(), player_character.get_position());

            controller.poll_input(Global::input);
            if (controller.face_away_from_camera())
            {
                DirectX::XMFLOAT3 f3;
                DirectX::XMStoreFloat3(&f3, camera.get_direction());

                f3.y = 0;

                player_character.set_twelve_oclock(DirectX::XMVector3NormalizeEst(DirectX::XMLoadFloat3(&f3)));
            }

            player_character.update(controller.get_character_controls());

            gfx.clear(bg_color);

            gfx.bind_depth_stencil_state(Gfx::DepthStencil::State::DEPTH_BUFFER);

            ib.bind();
            vbs.bind();
            vs.bind();

            ps.bind();

            cube.get_meshes()[0].constant_buffer.bind();

            vs.draw_indexed_instanced(0, cube.get_index_count(), 0, 1);

            // spider
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

            UINT prev_index = cube.get_meshes()[0].get_index_count(),
                 prev_vertex = cube.get_meshes()[0].get_vertex_count();

            // setting two_sided for this model because of "fur"
            gfx.set_rasterizer_state(Gfx::RasterizerState::TWO_SIDED);
            for (Mesh<TextureVertex> &m : spider.get_meshes())
            {
                UINT n = m.get_index_count();

                m.constant_buffer.bind();

                // Blender apparently doesn't export the two_sided fbx value...
                // gfx.set_rasterizer_state(m.is_two_sided() ? Gfx::RasterizerState::TWO_SIDED
                //                                           : Gfx::RasterizerState::STANDARD);

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
            for (Mesh<Vertex> &m : player_character.model.get_meshes())
            {
                UINT n = m.get_index_count();

                m.constant_buffer.bind();

                vs2.draw_indexed_instanced(prev_index, n, 2, 1, prev_vertex);

                prev_index += n;
                prev_vertex += m.get_vertex_count();
            }

            for (Mesh<Vertex> &m : grass_platform.get_meshes())
            {
                UINT n = m.get_index_count();

                m.constant_buffer.bind();

                vs2.draw_indexed_instanced(prev_index, n, 3, 1, prev_vertex);

                prev_index += n;
                prev_vertex += m.get_vertex_count();
            }

            instance_buffer.update(instance_data, sizeof(instance_data));

            // draw skybox
            gfx.bind_depth_stencil_state(Gfx::DepthStencil::State::ENVIRONMENT_BUFFER);
            gfx.set_rasterizer_state(Gfx::RasterizerState::TWO_SIDED);
            skybox.bind_and_draw();
            gfx.set_rasterizer_state(Gfx::RasterizerState::STANDARD);

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
