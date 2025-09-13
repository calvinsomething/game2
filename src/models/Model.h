#pragma once

#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <cstring>
#include <stdexcept>

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
            MESSAGE("num meshes " << c->mNumMeshes << " for " << c->mName.C_Str()
                                  << " with parent = " << node->mName.C_Str());
        }
        else
        {
            traverse_nodes(c);
        }
    }
}

template <typename T> class Model
{
  public:
    Model() : transform(DirectX::XMMatrixIdentity())
    {
    }

    Model(Gfx &gfx, std::string file_name, std::vector<T> &vertices, std::vector<uint32_t> &indices)
        : transform(DirectX::XMMatrixIdentity())
    {
        Assimp::Importer importer;

        scene = importer.ReadFile(file_name, aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                                 aiProcess_FlipWindingOrder | aiProcess_JoinIdenticalVertices |
                                                 aiProcess_SortByPType);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            throw std::runtime_error(importer.GetErrorString());
        }

        if (!scene->HasMeshes())
        {
            throw std::runtime_error("Model loaded with no meshes.");
        }

        meshes.reserve(scene->mNumMeshes);

        for (size_t i = 0; i < scene->mNumMeshes; ++i)
        {
            aiMesh &mesh = *scene->mMeshes[i];

            aiMaterial &material = *scene->mMaterials[mesh.mMaterialIndex];

            if (material.GetTextureCount(aiTextureType_DIFFUSE))
            {
                int diffuse_tc_index;

                material.Get(AI_MATKEY_UVWSRC(aiTextureType_DIFFUSE, 0), diffuse_tc_index);

                if (!mesh.HasTextureCoords(diffuse_tc_index))
                {
                    MESSAGE("Missing texture coords for " << file_name << " at index " << diffuse_tc_index);
                }

                aiString texture_file_name = {};
                if (material.Get(AI_MATKEY_TEXTURE(aiTextureType_DIFFUSE, 0), texture_file_name) == AI_SUCCESS)
                {
                    ai_texture = scene->GetEmbeddedTexture(texture_file_name.C_Str());
                    if (ai_texture)
                    {
                        // load embedded
                    }
                    else
                    {
                        size_t dir_end = file_name.find_last_of("/\\");

                        if (dir_end == std::string::npos)
                        {
                            MESSAGE("invalid file path: " << file_name);
                        }

                        std::string clean_texture_file_name = texture_file_name.C_Str();
                        {
                            size_t i = clean_texture_file_name.find_last_of(".");
                            if (i != std::string::npos)
                            {
                                clean_texture_file_name = clean_texture_file_name.substr(0, i);
                            }
                        }

                        std::string texture_path(file_name.substr(0, dir_end + 1) + clean_texture_file_name.c_str() +
                                                 ".dds");
                        for (char &c : texture_path)
                        {
                            if (c == '\\')
                            {
                                c = '/';
                            }
                        }

                        std::wstring w_path = to_wc(texture_path);

                        textures.reserve(textures.size() + 1);

                        textures.emplace_back(gfx, w_path.c_str());

                        meshes.emplace_back(mesh, vertices, indices, diffuse_tc_index, &textures, textures.size() - 1);
                    }
                }
                else
                {
                    // throw std::runtime_error("Failed to get diffuse texture file name.");
                    meshes.emplace_back(mesh, vertices, indices);
                }
            }
            else
            {
                meshes.emplace_back(mesh, vertices, indices);
            }
        }
    }

    void bind()
    {
    }

    void update(const DirectX::XMMATRIX &xform)
    {
        transform *= xform;
    }

    UINT get_index_count()
    {
        if (meshes.size())
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
    const aiScene *scene = 0;
    const aiTexture *ai_texture = 0;

    std::vector<Mesh<T>> meshes;
};
