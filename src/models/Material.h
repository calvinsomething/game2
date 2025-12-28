#pragma once

#include <assimp/material.h>
#include <assimp/scene.h>

#include "../gfx/Texture.h"
#include "../util.h"

struct Material
{
    DirectX::XMFLOAT4 color;

    Texture *diffuse_texture;
    Texture *normal_texture;
};

struct TextureCoordinateIndices
{
    size_t diffuse_coordinates_index;
    size_t normal_coordinates_index;
};

Material load_material(Gfx &gfx, const aiScene &scene, aiMaterial &ai_material,
                       TextureCoordinateIndices &coordinate_indices, StdVector<Texture> &textures,
                       StdUnorderedMap<std::string, size_t> &texture_index_by_file_name, const std::string &directory);
