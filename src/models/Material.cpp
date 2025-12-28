#pragma once

#include "Material.h"

#include <assimp/material.h>
#include <assimp/scene.h>

#include <iostream>

#include "../gfx/Gfx.h"
#include "../gfx/Texture.h"

// Textures
inline aiTextureType texture_types[] = {
    aiTextureType_DIFFUSE,
    aiTextureType_NORMALS,
};

// Colors
inline bool load_diffuse_color(aiMaterial &mat, aiColor3D &color)
{
    return mat.Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS;
}

constexpr size_t COLOR_LOADERS_COUNT = 1;

inline bool (*color_loaders[COLOR_LOADERS_COUNT])(aiMaterial &, aiColor3D &) = {load_diffuse_color};

Material load_material(Gfx &gfx, const aiScene &scene, aiMaterial &ai_material,
                       TextureCoordinateIndices &coordinate_indices, StdVector<Texture> &textures,
                       StdUnorderedMap<std::string, size_t> &texture_index_by_file_name, const std::string &directory)
{
    Material material = {};

    for (auto texture_type : texture_types)
    {
        if (ai_material.GetTextureCount(texture_type))
        {
            int tc_index;

            if (ai_material.Get(AI_MATKEY_UVWSRC(texture_type, 0), tc_index) != AI_SUCCESS)
            {
                throw std::runtime_error("Missing texture coordinates.");
            }

            aiString texture_file_name = {};
            if (ai_material.Get(AI_MATKEY_TEXTURE(texture_type, 0), texture_file_name) != AI_SUCCESS)
            {
                throw std::runtime_error("Failed to get diffuse texture file name.");
            }

            size_t texture_index;
            const char *texture_file_name_string = texture_file_name.C_Str();

            auto p_index = texture_index_by_file_name.find(texture_file_name_string);
            if (p_index != texture_index_by_file_name.end())
            {
                texture_index = p_index->second;
            }
            else
            {
                const aiTexture *ai_texture = scene.GetEmbeddedTexture(texture_file_name_string);
                if (ai_texture)
                {
                    // load embedded
                    std::cout << "embedded texture...\n";

                    continue;
                }
                else
                {
                    std::string clean_texture_file_name = texture_file_name_string;
                    {
                        size_t i = clean_texture_file_name.find_last_of(".");
                        if (i != std::string::npos)
                        {
                            clean_texture_file_name = clean_texture_file_name.substr(0, i);
                        }
                    }
                    std::cout << clean_texture_file_name << "\n";

                    std::string texture_path(directory + clean_texture_file_name.c_str() + ".dds");
                    for (char &c : texture_path)
                    {
                        if (c == '\\')
                        {
                            c = '/';
                        }
                    }

                    std::wstring w_path = to_wc(texture_path);

                    texture_index = textures.size();

                    textures.emplace_back(gfx, w_path.c_str());

                    texture_index_by_file_name.insert({texture_file_name_string, texture_index});
                }
            }

            switch (texture_type)
            {
            case aiTextureType_DIFFUSE:
                coordinate_indices.diffuse_coordinates_index = (size_t)tc_index;
                material.diffuse_texture = &textures[texture_index];
                break;
            case aiTextureType_NORMALS:
                coordinate_indices.normal_coordinates_index = (size_t)tc_index;
                material.normal_texture = &textures[texture_index];
                break;
            default:
                break;
            }
        }
    }

    DirectX::XMFLOAT4 colors[COLOR_LOADERS_COUNT];
    for (size_t i = 0; i < COLOR_LOADERS_COUNT; ++i)
    {
        aiColor3D color(0.f, 0.f, 0.f);
        if (color_loaders[i](ai_material, color))
        {
            colors[i].x = color.r;
            colors[i].y = color.g;
            colors[i].z = color.b;
            colors[i].w = 1.0f;
        }
        else
        {
            colors[i].x = 1.0f;
            colors[i].y = 1.0f;
            colors[i].z = 1.0f;
            colors[i].w = 1.0f;
        }
    }

    material.color = colors[0];

    return material;
}
