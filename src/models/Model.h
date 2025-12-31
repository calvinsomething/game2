#pragma once

#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <chrono>
#include <cstring>
#include <stdexcept>

#include "../Gfx/Texture.h"
#include "../Gfx/VertexShader.h"
#include "Material.h"
#include "Mesh.h"

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

    Model(Gfx &gfx, const std::string &file_name, StdVector<T> &vertices, StdVector<uint32_t> &indices,
          StdVector<Material> &materials, StdVector<Texture> &textures)
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

        std::string directory = directory_from_file_name(file_name);
        StdUnorderedMap<std::string, size_t> texture_index_by_file_name;

        StdVector<TextureCoordinateIndices> coordinate_indices(scene->mNumMaterials, TextureCoordinateIndices{});
        StdVector<int> two_sided(scene->mNumMaterials, 0);

        size_t material_index_start = materials.size();

        for (size_t i = 0; i < scene->mNumMaterials; ++i)
        {
            materials.push_back(load_material(gfx, *scene, *scene->mMaterials[i], coordinate_indices[i], textures,
                                              texture_index_by_file_name, directory, two_sided[i]));
        }

        meshes.reserve(scene->mNumMeshes);
        for (size_t i = 0; i < scene->mNumMeshes; ++i)
        {
            aiMesh &mesh = *scene->mMeshes[i];

            meshes.emplace_back(gfx, mesh, vertices, indices, material_index_start + mesh.mMaterialIndex,
                                coordinate_indices[mesh.mMaterialIndex], !!two_sided[mesh.mMaterialIndex]);

            Mesh<T> &m = meshes.back();

            if (animation)
            {
                m.load_bones(mesh, vertices);
            }
        }
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
        UINT count = 0;

        for (auto &m : meshes)
        {
            count += m.get_index_count();
        }

        return count;
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

    StdVector<Mesh<T>> &get_meshes()
    {
        return meshes;
    }

  protected:
    // TODO rework this... maybe quaternion?
    DirectX::XMMATRIX transform;

    DirectX::XMFLOAT3 position;

    VertexShader::BufferData buffer_data;

    StdVector<Mesh<T>> meshes;

  private:
    Assimp::Importer importer;

    const aiScene *scene = 0;
    const aiTexture *ai_texture = 0;

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<decltype(clock)> animation_start_time = {};

    StdUnorderedMap<const aiNode *, const aiNodeAnim *> node_animations;
};
