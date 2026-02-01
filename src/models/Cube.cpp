#include "Cube.h"

#include <cassert>

DirectX::XMFLOAT3 cube_positions[] = {
    {-5.0f, 5.0f, 5.0f},  {5.0f, 5.0f, 5.0f},   {-5.0f, 5.0f, 5.0f},   {-5.0f, 5.0f, -5.0f}, {5.0f, 5.0f, -5.0f},
    {5.0f, 5.0f, 5.0f},   {-5.0f, -5.0f, 5.0f}, {-5.0f, -5.0f, -5.0f}, {5.0f, -5.0f, -5.0f}, {5.0f, -5.0f, 5.0f},
    {-5.0f, -5.0f, 5.0f}, {5.0f, -5.0f, 5.0f},  {-5.0f, 5.0f, 5.0f},   {5.0f, 5.0f, 5.0f},
};

extern const size_t CUBE_POSITIONS_COUNT = ARRAYSIZE(cube_positions);

DirectX::XMFLOAT3 (&pos)[CUBE_POSITIONS_COUNT] = cube_positions;

TextureVertex vertices[CUBE_POSITIONS_COUNT] = {
    {pos[0], normalize(pos[0]), {}, {}, TextureCoordinates{DirectX::XMFLOAT2(0.33f, 0.0f)}},
    {pos[1], normalize(pos[1]), {}, {}, {DirectX::XMFLOAT2(0.67f, 0.0f)}},
    {pos[2], normalize(pos[2]), {}, {}, {DirectX::XMFLOAT2(0.0f, 0.25f)}},
    {pos[3], normalize(pos[3]), {}, {}, {DirectX::XMFLOAT2(0.33f, 0.25f)}},
    {pos[4], normalize(pos[4]), {}, {}, {DirectX::XMFLOAT2(0.67f, 0.25f)}},
    {pos[5], normalize(pos[5]), {}, {}, {DirectX::XMFLOAT2(1.0f, 0.25f)}},
    {pos[6], normalize(pos[6]), {}, {}, {DirectX::XMFLOAT2(0.0f, 0.5f)}},
    {pos[7], normalize(pos[7]), {}, {}, {DirectX::XMFLOAT2(0.33f, 0.5f)}},
    {pos[8], normalize(pos[8]), {}, {}, {DirectX::XMFLOAT2(0.67f, 0.5f)}},
    {pos[9], normalize(pos[9]), {}, {}, {DirectX::XMFLOAT2(1.0f, 0.5f)}},
    {pos[10], normalize(pos[10]), {}, {}, {DirectX::XMFLOAT2(0.33f, 0.75f)}},
    {pos[11], normalize(pos[11]), {}, {}, {DirectX::XMFLOAT2(0.67f, 0.75f)}},
    {pos[12], normalize(pos[12]), {}, {}, {DirectX::XMFLOAT2(0.33f, 1.0f)}},
    {pos[13], normalize(pos[13]), {}, {}, {DirectX::XMFLOAT2(0.67f, 1.0f)}}};

uint32_t cube_indices[] = {0, 1, 4, 4, 3, 0, 2, 3, 7,  7,  6,  2, 3,  4,  8,  8,  7,  3,
                           4, 5, 9, 9, 8, 4, 7, 8, 11, 11, 10, 7, 10, 11, 13, 13, 12, 10};
extern const size_t CUBE_INDICES_COUNT = ARRAYSIZE(cube_indices);

Cube::Cube(Gfx &gfx, const std::wstring &texture_file_name, StdVector<TextureVertex> &vertices,
           StdVector<uint32_t> &indices, StdVector<Material> &materials, StdVector<Texture2D> &textures,
           InstanceData &instance_data)
    : Model(instance_data)
{
    size_t start_vertex = load_vector(vertices, ::vertices, CUBE_POSITIONS_COUNT);

    size_t start_index = load_vector(indices, ::cube_indices, CUBE_INDICES_COUNT);

    meshes.emplace_back(gfx, start_vertex, CUBE_POSITIONS_COUNT, start_index, CUBE_INDICES_COUNT, materials.size());

    Material mat = {};
    mat.diffuse_texture_index = textures.size();

    textures.emplace_back(gfx, texture_file_name.c_str());

    materials.push_back(mat);
}
