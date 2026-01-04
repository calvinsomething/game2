#include "Cube.h"

#include <cassert>

DirectX::XMFLOAT3 positions[] = {
    {-5.0f, 5.0f, 5.0f},  {5.0f, 5.0f, 5.0f},   {-5.0f, 5.0f, 5.0f},   {-5.0f, 5.0f, -5.0f}, {5.0f, 5.0f, -5.0f},
    {5.0f, 5.0f, 5.0f},   {-5.0f, -5.0f, 5.0f}, {-5.0f, -5.0f, -5.0f}, {5.0f, -5.0f, -5.0f}, {5.0f, -5.0f, 5.0f},
    {-5.0f, -5.0f, 5.0f}, {5.0f, -5.0f, 5.0f},  {-5.0f, 5.0f, 5.0f},   {5.0f, 5.0f, 5.0f},
};

TextureVertex vertices[] = {
    {positions[0], normalize(positions[0]), {}, {}, TextureCoordinates{DirectX::XMFLOAT2(0.33f, 0.0f)}},
    {positions[1], normalize(positions[1]), {}, {}, {DirectX::XMFLOAT2(0.67f, 0.0f)}},
    {positions[2], normalize(positions[2]), {}, {}, {DirectX::XMFLOAT2(0.0f, 0.25f)}},
    {positions[3], normalize(positions[3]), {}, {}, {DirectX::XMFLOAT2(0.33f, 0.25f)}},
    {positions[4], normalize(positions[4]), {}, {}, {DirectX::XMFLOAT2(0.67f, 0.25f)}},
    {positions[5], normalize(positions[5]), {}, {}, {DirectX::XMFLOAT2(1.0f, 0.25f)}},
    {positions[6], normalize(positions[6]), {}, {}, {DirectX::XMFLOAT2(0.0f, 0.5f)}},
    {positions[7], normalize(positions[7]), {}, {}, {DirectX::XMFLOAT2(0.33f, 0.5f)}},
    {positions[8], normalize(positions[8]), {}, {}, {DirectX::XMFLOAT2(0.67f, 0.5f)}},
    {positions[9], normalize(positions[9]), {}, {}, {DirectX::XMFLOAT2(1.0f, 0.5f)}},
    {positions[10], normalize(positions[10]), {}, {}, {DirectX::XMFLOAT2(0.33f, 0.75f)}},
    {positions[11], normalize(positions[11]), {}, {}, {DirectX::XMFLOAT2(0.67f, 0.75f)}},
    {positions[12], normalize(positions[12]), {}, {}, {DirectX::XMFLOAT2(0.33f, 1.0f)}},
    {positions[13], normalize(positions[13]), {}, {}, {DirectX::XMFLOAT2(0.67f, 1.0f)}}};

uint32_t indices[] = {0, 1, 4, 4, 3, 0, 2, 3, 7,  7,  6,  2, 3,  4,  8,  8,  7,  3,
                      4, 5, 9, 9, 8, 4, 7, 8, 11, 11, 10, 7, 10, 11, 13, 13, 12, 10};

Cube::Cube(Gfx &gfx, const std::wstring &texture_file_name, StdVector<TextureVertex> &vertices,
           StdVector<uint32_t> &indices, StdVector<Material> &materials, StdVector<Texture> &textures,
           InstanceData &instance_data)
    : Model(instance_data)
{
    size_t vertex_count = ARRAYSIZE(::vertices), start_vertex = load_vector(vertices, ::vertices, vertex_count);

    size_t index_count = ARRAYSIZE(::indices), start_index = load_vector(indices, ::indices, index_count);

    meshes.emplace_back(gfx, start_vertex, vertex_count, start_index, index_count, materials.size());

    Material mat = {};
    mat.diffuse_texture_index = textures.size();

    textures.emplace_back(gfx, texture_file_name.c_str());

    materials.push_back(mat);
}
