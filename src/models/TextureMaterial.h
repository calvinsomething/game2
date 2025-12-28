#pragma once

#include "Material.h"

#include <assimp/material.h>
#include <assimp/scene.h>

#include <iostream>

#include "../gfx/Gfx.h"
#include "../gfx/Texture.h"
#include "../util.h"

// Textures
inline aiTextureType texture_types[] = {
    aiTextureType_AMBIENT,
    aiTextureType_DIFFUSE,
    aiTextureType_NORMALS,
};

// Colors
inline bool load_ambient_color(aiMaterial &mat, aiColor3D &color)
{
    return mat.Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS;
}

inline bool load_diffuse_color(aiMaterial &mat, aiColor3D &color)
{
    return mat.Get(AI_MATKEY_COLOR_DIFFUSE, color) == aiReturn_SUCCESS;
}

constexpr size_t COLOR_LOADERS_COUNT = 2;

inline bool (*color_loaders[COLOR_LOADERS_COUNT])(aiMaterial &, aiColor3D &) = {load_ambient_color, load_diffuse_color};

class TextureMaterial : public Material
{
    friend Material load_material(Gfx &gfx, aiScene &scene, aiMaterial &ai_material, StdVector<Texture> &textures,
                                  StdUnorderedMap<std::string, size_t> texture_index_by_file_name,
                                  const std::string &directory);

  public:
    TextureMaterial(Gfx &gfx, Texture *ambient_map, int ambient_map_index, Texture *diffuse_map, int diffuse_map_index,
                    Texture *normal_map, int normal_map_index)
        : Material(gfx)
    {
        data.maps.ambient.texture = ambient_map;
        data.maps.ambient.coordinates_index = ambient_map_index;
        data.maps.diffuse.texture = diffuse_map;
        data.maps.diffuse.coordinates_index = diffuse_map_index;
        data.maps.normal.texture = normal_map;
        data.maps.normal.coordinates_index = normal_map_index;
    }
};

class FlatColorMaterial : public Material
{
    friend Material load_material(Gfx &gfx, aiScene &scene, aiMaterial &ai_material, StdVector<Texture> &textures,
                                  StdUnorderedMap<std::string, size_t> texture_index_by_file_name,
                                  const std::string &directory);

  public:
    FlatColorMaterial(Gfx &gfx, DirectX::XMFLOAT4 ambient_color, DirectX::XMFLOAT4 diffuse_color) : Material(gfx)
    {
        data.colors.ambient_color = ambient_color;
        data.colors.diffuse_color = diffuse_color;
    }
};

Material load_material(Gfx &gfx, aiScene &scene, aiMaterial &ai_material, StdVector<Texture> &textures,
                       StdUnorderedMap<std::string, size_t> texture_index_by_file_name, const std::string &directory)
{
    int ambient_map_index, diffuse_map_index, normal_map_index;
    Texture *ambient_map = 0, *diffuse_map = 0, *normal_map = 0;

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
            case aiTextureType_AMBIENT:
                ambient_map_index = tc_index;
                ambient_map = &textures.back();
                break;
            case aiTextureType_DIFFUSE:
                diffuse_map_index = tc_index;
                diffuse_map = &textures.back();
                break;
            case aiTextureType_NORMALS:
                normal_map_index = tc_index;
                normal_map = &textures.back();
                break;
            default:
                break;
            }
        }
    }

    if (ambient_map || diffuse_map || normal_map)
    {
        return TextureMaterial(gfx, ambient_map, ambient_map_index, diffuse_map, diffuse_map_index, normal_map,
                               normal_map_index);
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

    return FlatColorMaterial(gfx, colors[0], colors[1]);
}
