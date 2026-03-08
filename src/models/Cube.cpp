#include "Cube.h"

#include <cassert>

DirectX::XMFLOAT3 cube_positions[] = {
    {-5.0f, 5.0f, -5.0f}, {5.0f, 5.0f, -5.0f}, {5.0f, -5.0f, -5.0f}, {-5.0f, -5.0f, -5.0f},
    {5.0f, 5.0f, 5.0f},   {-5.0f, 5.0f, 5.0f}, {-5.0f, -5.0f, 5.0f}, {5.0f, -5.0f, 5.0f},
};

DirectX::XMFLOAT3 (&pos)[8] = cube_positions;

TextureVertex vertices[] = {
    // front
    {pos[0], {0.0f, 0.0f, -1.0f}, {}, {}, {DirectX::XMFLOAT2(0.33f, 0.25f)}},
    {pos[1], {0.0f, 0.0f, -1.0f}, {}, {}, {DirectX::XMFLOAT2(0.67f, 0.25f)}},
    {pos[2], {0.0f, 0.0f, -1.0f}, {}, {}, {DirectX::XMFLOAT2(0.67f, 0.50f)}},
    {pos[3], {0.0f, 0.0f, -1.0f}, {}, {}, {DirectX::XMFLOAT2(0.33f, 0.50f)}},
    // back
    {pos[4], {0.0f, 0.0f, 1.0f}, {}, {}, {DirectX::XMFLOAT2(0.67f, 0.75f)}},
    {pos[5], {0.0f, 0.0f, 1.0f}, {}, {}, {DirectX::XMFLOAT2(0.33f, 0.75f)}},
    {pos[6], {0.0f, 0.0f, 1.0f}, {}, {}, {DirectX::XMFLOAT2(0.33f, 1.00f)}},
    {pos[7], {0.0f, 0.0f, 1.0f}, {}, {}, {DirectX::XMFLOAT2(0.67f, 1.00f)}},
    // left
    {pos[5], {-1.0f, 0.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.00f, 0.25f)}},
    {pos[0], {-1.0f, 0.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.33f, 0.25f)}},
    {pos[3], {-1.0f, 0.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.33f, 0.50f)}},
    {pos[6], {-1.0f, 0.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.00f, 0.50f)}},
    // right
    {pos[1], {1.0f, 0.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.67f, 0.25f)}},
    {pos[4], {1.0f, 0.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(1.00f, 0.25f)}},
    {pos[7], {1.0f, 0.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(1.00f, 0.50f)}},
    {pos[2], {1.0f, 0.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.67f, 0.50f)}},
    // top
    {pos[5], {0.0f, 1.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.33f, 0.00f)}},
    {pos[4], {0.0f, 1.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.67f, 0.00f)}},
    {pos[1], {0.0f, 1.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.67f, 0.25f)}},
    {pos[0], {0.0f, 1.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.33f, 0.25f)}},
    // bottom
    {pos[3], {0.0f, -1.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.33f, 0.50f)}},
    {pos[2], {0.0f, -1.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.67f, 0.50f)}},
    {pos[7], {0.0f, -1.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.67f, 0.75f)}},
    {pos[6], {0.0f, -1.0f, 0.0f}, {}, {}, {DirectX::XMFLOAT2(0.33f, 0.75f)}},
};

static uint32_t cube_indices[] = {
    0,  1,  2,  0,  2,  3,  // front
    4,  5,  6,  4,  6,  7,  // back
    8,  9,  10, 8,  10, 11, // left
    12, 13, 14, 12, 14, 15, // right
    16, 17, 18, 16, 18, 19, // top
    20, 21, 22, 20, 22, 23, // bottom
};

Cube::Cube(Gfx &gfx, const std::wstring &texture_file_name, StdVector<TextureVertex> &vertices,
           StdVector<uint32_t> &indices, StdVector<Material> &materials, StdVector<Texture2D> &textures,
           InstanceData &instance_data)
    : Model(instance_data)
{
    size_t start_vertex = load_vector(vertices, ::vertices, ARRAYSIZE(::vertices));

    size_t start_index = load_vector(indices, cube_indices, ARRAYSIZE(cube_indices));

    meshes.emplace_back(gfx, start_vertex, ARRAYSIZE(::vertices), start_index, ARRAYSIZE(cube_indices),
                        materials.size());

    Material mat = {};
    mat.diffuse_texture_index = textures.size();
    mat.roughness = 0.6f;

    textures.emplace_back(gfx, texture_file_name.c_str());

    materials.push_back(mat);
}
