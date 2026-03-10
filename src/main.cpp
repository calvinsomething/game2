#include <DirectXMath.h>
#include <windows.h>

#include <exception>

#include "init.h"

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

float bg_color[] = {0.15f, 0.15f, 0.2f, 1.0f};

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
    Window window;

    RECT rect = window.get_rect();
    Global::client_width = rect.right - rect.left;
    Global::client_height = rect.bottom - rect.top;
    Global::client_aspect_ratio = float(Global::client_width) / Global::client_height;

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

        StdVector<TextureVertex> *vertices = Allocator<StdVector<TextureVertex>>::create();
        vertices->reserve(4096);

        StdVector<Vertex> *vertices2 = Allocator<StdVector<Vertex>>::create();
        vertices2->reserve(4096);

        StdVector<uint32_t> *indices = Allocator<StdVector<uint32_t>>::create();
        indices->reserve(4096);

        StdVector<uint32_t> *indices2 = Allocator<StdVector<uint32_t>>::create();
        indices2->reserve(4096);

        StdVector<Material> *materials = Allocator<StdVector<Material>>::create();
        materials->reserve(64);

        StdVector<Texture2D> textures;
        textures.reserve(64);

        Global::input.register_devices();

        Gfx gfx(window.get_handle());

        Camera camera(gfx);

        Controller controller;

        ConstantBuffer lighting_buffer(gfx, ConstantBuffer::Slot::LIGHTING_BUFFER, ConstantBuffer::bind_vs_and_ps,
                                       Global::lighting_data); // bound as static/read-only
        lighting_buffer.bind();

        InstanceData instance_data[4]{};

        Cube cube(gfx, L"assets/textures/minecraft_cube.dds", *vertices, *indices, *materials, textures,
                  instance_data[0]);
        cube.set_correction_transform(DirectX::XMMatrixMultiply(
            DirectX::XMMatrixRotationQuaternion(DirectX::XMVECTOR{std::sqrt(0.5f), 0.0f, 0.0f, -std::sqrt(0.5f)}),
            DirectX::XMMatrixTranslation(0.0f, 5.0f, 0.0f)));

        Model spider(gfx, "assets/models/spider/spider.fbx", *vertices, *indices, *materials, textures,
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

        for (Mesh<Vertex> &m : grass_platform.get_meshes())
        {
            Global::collision_system.load_floor_triangles(&vertices2->at(m.get_start_vertex()),
                                                          &indices2->at(m.get_start_index()), m.get_index_count(),
                                                          grass_platform.get_transform());
        }

        Character player_character(ninja);
        player_character.set_position({0.0f, 0.0f, -8.0f});

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

        ShadowMapVertexShader vs_shadow(gfx);

        // Skybox
        Skybox skybox(gfx, "assets/hdr/PlanetaryEarth4k.hdr");

        ps.set_textures(textures);

        // deallocate
        Allocator<StdVector<TextureVertex>>::release(vertices);
        vertices = 0;

        Allocator<StdVector<Vertex>>::release(vertices2);
        vertices2 = 0;

        Allocator<StdVector<uint32_t>>::release(indices);
        indices = 0;

        Allocator<StdVector<uint32_t>>::release(indices2);
        indices2 = 0;

        Allocator<StdVector<Material>>::release(materials);
        materials = 0;
        //

        Buffer *v_buffers[] = {&vb, &instance_buffer};
        VertexBuffers vbs(gfx, v_buffers);

        v_buffers[0] = &vb2;
        VertexBuffers vbs2(gfx, v_buffers);

        bone_data_buffer.bind();

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

            spider.update();

            player_character.set_target(spider);
            player_character.update(controller.get_character_controls());

            // bone data
            bone_data_buffer.start_batch_update();

            size_t animation_data_offset = spider.update_bone_buffer(bone_data_buffer, 0);
            ninja.update_bone_buffer(bone_data_buffer, animation_data_offset);

            bone_data_buffer.end_batch_update();
            //

            gfx.clear(bg_color);

            // Shadow map pass
            {
                gfx.set_render_target(Gfx::RenderTarget::SHADOW_MAP);
                gfx.set_rasterizer_state(Gfx::RasterizerState::SHADOW_MAP);

                vs_shadow.bind(vs2.get_input_layout());
                ib2.bind();
                vbs2.bind();

                size_t prev_index = 0, prev_vertex = 0;
                player_character.model.draw_indexed_instanced(vs_shadow, prev_index, prev_vertex, 2);

                vs_shadow.bind_tex(vs.get_input_layout());
                ib.bind();
                vbs.bind();

                prev_index = 0;
                prev_vertex = 0;

                cube.draw_indexed_instanced(vs_shadow, prev_index, prev_vertex, 0);
                vs_shadow.draw_indexed_instanced(0, cube.get_index_count(), 0, 1);

                prev_index = cube.get_meshes()[0].get_index_count();
                prev_vertex = cube.get_meshes()[0].get_vertex_count();

                spider.draw_indexed_instanced(vs_shadow, prev_index, prev_vertex, 1);
            }

            gfx.set_render_target(Gfx::RenderTarget::MAIN);
            gfx.set_rasterizer_state(Gfx::RasterizerState::STANDARD);
            gfx.bind_depth_stencil_state(Gfx::DepthStencil::State::DEPTH_BUFFER);

            vs.bind();
            ps.bind();

            size_t prev_index = 0, prev_vertex = 0;

            cube.draw_indexed_instanced(vs, prev_index, prev_vertex, 0);

            // setting two_sided for this model because of "fur"
            gfx.set_rasterizer_state(Gfx::RasterizerState::TWO_SIDED);

            spider.draw_indexed_instanced(vs_shadow, prev_index, prev_vertex, 1);

            gfx.set_rasterizer_state(Gfx::RasterizerState::STANDARD);

            // ninja
            ps2.bind();
            vs2.bind();
            ib2.bind();
            vbs2.bind();

            prev_index = 0;
            prev_vertex = 0;

            // flat shaded
            player_character.model.draw_indexed_instanced(vs2, prev_index, prev_vertex, 2);
            grass_platform.draw_indexed_instanced(vs2, prev_index, prev_vertex, 3);

            instance_buffer.update(instance_data, sizeof(instance_data));

            // draw skybox
            gfx.bind_depth_stencil_state(Gfx::DepthStencil::State::ENVIRONMENT_BUFFER);
            gfx.set_rasterizer_state(Gfx::RasterizerState::TWO_SIDED);
            skybox.bind_and_draw();

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
