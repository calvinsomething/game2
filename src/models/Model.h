#pragma once

#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <chrono>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <unordered_map>

#include "../Error.h"
#include "../Gfx/Texture.h"
#include "../Gfx/VertexShader.h"
#include "../util.h"
#include "Mesh.h"

inline void traverse_nodes(aiNode *node)
{
    for (size_t i = 0; i < node->mNumChildren; ++i)
    {
        aiNode *c = node->mChildren[i];

        if (c->mNumMeshes && strcmp(c->mName.C_Str(), "Box"))
        {
            ERROR_MSG("num meshes " << c->mNumMeshes << " for " << c->mName.C_Str()
                                    << " with parent = " << node->mName.C_Str());
        }
        else
        {
            traverse_nodes(c);
        }
    }
}

inline aiTextureType texture_types[] = {
    aiTextureType_DIFFUSE,
    aiTextureType_NORMALS,
};

template <typename T> class Model
{
  public:
    Model() : transform(DirectX::XMMatrixIdentity())
    {
    }

    Model(Gfx &gfx, const std::string &file_name, std::vector<T> &vertices, std::vector<uint32_t> &indices)
        : transform(DirectX::XMMatrixIdentity())
    {
        scene = importer.ReadFile(file_name, aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                                 aiProcess_FlipWindingOrder | aiProcess_JoinIdenticalVertices |
                                                 aiProcess_SortByPType | aiProcess_FlipUVs |
                                                 aiProcess_PopulateArmatureData);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            throw std::runtime_error(importer.GetErrorString());
        }

        if (!scene->HasMeshes())
        {
            throw std::runtime_error("Model loaded with no meshes.");
        }

        meshes.reserve(scene->mNumMeshes);

        aiAnimation *animation = 0;
        if (scene->HasAnimations())
        {
            animation = scene->mAnimations[0];

            for (size_t i = 0; i < animation->mNumChannels; ++i)
            {
                aiNode *node = scene->mRootNode->FindNode(animation->mChannels[i]->mNodeName);
                if (node)
                {
                    node_animations.insert({node, animation->mChannels[i]});
                }
            }
        }

        start_vertex = vertices.size();
        start_index = indices.size();

        for (size_t i = 0; i < scene->mNumMeshes; ++i)
        {
            aiMesh &mesh = *scene->mMeshes[i];

            aiMaterial &material = *scene->mMaterials[mesh.mMaterialIndex];

            if (!load_texture(gfx, file_name, vertices, indices, mesh, material))
            {
                meshes.emplace_back(mesh, vertices, start_vertex, indices, start_index);
            }

            Mesh<T> &m = meshes.back();

            if (animation)
            {
                m.load_bones(mesh, vertices);
            }
        }
    }

    bool load_texture(Gfx &gfx, const std::string &file_name, std::vector<T> &vertices, std::vector<uint32_t> &indices,
                      aiMesh &mesh, aiMaterial &material)
    {

        for (auto texture_type : texture_types)
        {
            if (material.GetTextureCount(texture_type))
            {
                int tc_index;

                material.Get(AI_MATKEY_UVWSRC(texture_type, 0), tc_index);

                if (!mesh.HasTextureCoords(tc_index))
                {
                    ERROR_MSG("Missing texture coords for " << file_name << " at index " << tc_index);
                }

                aiString texture_file_name = {};
                if (material.Get(AI_MATKEY_TEXTURE(texture_type, 0), texture_file_name) == AI_SUCCESS)
                {
                    size_t texture_index;

                    const char *texture_file_name_string = texture_file_name.C_Str();

                    auto p_index = texture_indices_by_file_name.find(texture_file_name_string);
                    if (p_index != texture_indices_by_file_name.end())
                    {
                        texture_index = p_index->second;
                        std::cout << "texture loaded " << texture_index << texture_file_name_string << "\n";
                    }
                    else
                    {
                        ai_texture = scene->GetEmbeddedTexture(texture_file_name_string);
                        if (ai_texture)
                        {
                            // load embedded
                            std::cout << "embedded texture...\n";

                            texture_index = 0;
                        }
                        else
                        {
                            size_t dir_end = file_name.find_last_of("/\\");

                            if (dir_end == std::string::npos)
                            {
                                ERROR_MSG("invalid file path: " << file_name);
                            }

                            std::string clean_texture_file_name = texture_file_name_string;
                            {
                                size_t i = clean_texture_file_name.find_last_of(".");
                                if (i != std::string::npos)
                                {
                                    clean_texture_file_name = clean_texture_file_name.substr(0, i);
                                }
                            }

                            std::string texture_path(file_name.substr(0, dir_end + 1) +
                                                     clean_texture_file_name.c_str() + ".dds");
                            for (char &c : texture_path)
                            {
                                if (c == '\\')
                                {
                                    c = '/';
                                }
                            }

                            std::wstring w_path = to_wc(texture_path);

                            std::wcout << mesh.mName.C_Str() << " " << w_path << L"\n";

                            textures.reserve(textures.size() + 1);

                            texture_index = textures.size();

                            textures.emplace_back(gfx, w_path.c_str());

                            texture_indices_by_file_name.insert({texture_file_name_string, texture_index});
                        }
                    }

                    meshes.emplace_back(mesh, vertices, indices, tc_index, &textures, texture_index);
                }
                else
                {
                    throw std::runtime_error("Failed to get diffuse texture file name.");
                }
                return true;
            }
        }

        return false;
    }

    void bind()
    {
    }

    void start_animation()
    {
        animation_start_time = clock.now();
    }

    void update(const DirectX::XMMATRIX &xform)
    {
        transform *= xform;

        if (animation_start_time.time_since_epoch().count())
        {
            double animation_time_sec = ((clock.now() - animation_start_time).count()) * 0.000000001;

            double time_in_ticks = scene->mAnimations[0]->mTicksPerSecond * animation_time_sec;

            if (time_in_ticks >= 0 && time_in_ticks <= scene->mAnimations[0]->mDuration)
            {
                for (auto &m : meshes)
                {
                    m.animate(node_animations, time_in_ticks);
                }
            }
            else
            {
                animation_start_time = {};
            }
        }
    }

    UINT get_index_count()
    {
        if (!meshes.empty())
        {
            UINT count = 0;

            for (auto &m : meshes)
            {
                count += m.get_index_count();
            }

            return count;
        }

        return UINT(index_count);
    }

    UINT get_vertex_count()
    {
        return UINT(vertex_count);
    }

    void set_position(float x, float y, float z)
    {
        position = DirectX::XMFLOAT3(x, y, z);
    }

    void translate(float x, float y, float z)
    {
        position.x += x;
        position.y += y;
        position.z += z;
    }

    DirectX::XMMATRIX get_transform()
    {
        return transform * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    }

    const aiTexture *get_ai_texture()
    {
        return ai_texture;
    }

    std::vector<Texture> &get_textures()
    {
        return textures;
    }

    size_t load_texture(Gfx &gfx, const wchar_t *file_name)
    {
        textures.reserve(textures.size() + 1);

        textures.emplace_back(gfx, file_name);

        return textures.size() - 1;
    }

    std::vector<Mesh<T>> &get_meshes()
    {
        return meshes;
    }

  protected:
    size_t start_vertex, vertex_count, start_index, index_count;

    // TODO rework this... maybe quaternion?
    DirectX::XMMATRIX transform;

    DirectX::XMFLOAT3 position;

    VertexShader::BufferData buffer_data;

    std::vector<Texture> textures;

  private:
    Assimp::Importer importer;

    const aiScene *scene = 0;
    const aiTexture *ai_texture = 0;

    std::vector<Mesh<T>> meshes;

    std::unordered_map<std::string, size_t> texture_indices_by_file_name;

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<decltype(clock)> animation_start_time = {};

    std::unordered_map<std::string, aiAnimation *> animations;

    std::unordered_map<const aiNode *, const aiNodeAnim *> node_animations;
};
