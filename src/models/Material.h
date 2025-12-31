#pragma once

#include <assimp/material.h>
#include <assimp/scene.h>

#include "../gfx/Texture.h"
#include "../util.h"

struct Material
{
    DirectX::XMFLOAT4 color;
    int32_t diffuse_texture_index = -1;
    int32_t normal_texture_index = -1;
    float roughness = 1.0f;
};

struct TextureCoordinateIndices
{
    int diffuse_coordinates_index = -1;
    int normal_coordinates_index = -1;
};

Material load_material(Gfx &gfx, const aiScene &scene, aiMaterial &ai_material,
                       TextureCoordinateIndices &coordinate_indices, StdVector<Texture> &textures,
                       StdUnorderedMap<std::string, size_t> &texture_index_by_file_name, const std::string &directory,
                       int &two_sided);
