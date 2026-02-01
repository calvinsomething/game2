#pragma once

#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <chrono>
#include <cstring>
#include <stdexcept>

#include "../gfx/InstanceBuffer.h"
#include "../gfx/Texture.h"
#include "../gfx/VertexShader.h"
#include "Animations.h"
#include "Material.h"
#include "Mesh.h"

inline aiTextureType texture_types[] = {
    aiTextureType_DIFFUSE,
    aiTextureType_NORMALS,
};

template <typename T> class Model
{
  public:
    Model(InstanceData &instance_data) : instance_data(instance_data)
    {
    }

    Model(Gfx &gfx, const std::string &file_name, StdVector<T> &vertices, StdVector<uint32_t> &indices,
          StdVector<Material> &materials, StdVector<Texture2D> &textures, InstanceData &instance_data,
          uint32_t bone_start = 0)
        : instance_data(instance_data)
    {
        instance_data.bone_start = bone_start;

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

        if (scene->HasAnimations())
        {
            animations = Animations(scene);
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

            if (scene->HasAnimations())
            {
                m.load_bones(mesh, vertices);
            }

            bone_count += m.bone_matrices.size();
        }
    }

    void loop_animation(const std::string &name)
    {
        start_animation(name);

        animations.set_looping(true);
    }

    void start_animation(const std::string &name)
    {
        animations.set_animation(name);

        animation_start_time = clock.now();
    }

    bool is_animating()
    {
        return animations.get();
    }

    void update()
    {
        Animation *animation = animations.get();

        if (animation)
        {
            double animation_time_sec =
                (clock.now() - animation_start_time).count() * 0.000000001; // nanoseconds to seconds

            double time_in_ticks = animation->ticks_per_second * animation_time_sec;

            if (time_in_ticks > animation->duration && animations.is_looping())
            {
                animation_start_time = clock.now();
                time_in_ticks = 0;
            }

            if (time_in_ticks <= animation->duration)
            {
                for (auto &m : meshes)
                {
                    m.animate(animation->node_anim_map, time_in_ticks);
                }
            }
            else
            {
                animation_start_time = {};
                animations.unset();
            }
        }

        instance_data.transform = DirectX::XMMatrixTranspose(get_transform());
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

    void set_position(DirectX::XMFLOAT3 pos)
    {
        position = pos;
    }

    void translate(float x, float y, float z)
    {
        position.x += x;
        position.y += y;
        position.z += z;
    }

    void scale(float x, float y, float z)
    {
        scaling *= DirectX::XMMatrixScaling(x, y, z);
    }

    void set_base_transform(const DirectX::XMMATRIX &xform)
    {
        base_transform = xform;
    }

    DirectX::XMMATRIX get_transform()
    {
        return base_transform * scaling * DirectX::XMMatrixRotationQuaternion(orientation) *
               DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    }

    const aiTexture *get_ai_texture()
    {
        return ai_texture;
    }

    StdVector<Mesh<T>> &get_meshes()
    {
        return meshes;
    }

    uint32_t get_bone_count()
    {
        return bone_count;
    }

    void rotate(DirectX::XMVECTOR rotation)
    {
        orientation = DirectX::XMQuaternionMultiply(orientation, rotation);
    }

    void set_orientation(DirectX::XMVECTOR orientation)
    {
        Model::orientation = orientation;
    }

  protected:
    DirectX::XMMATRIX base_transform = DirectX::XMMatrixIdentity();

    InstanceData &instance_data;
    uint32_t bone_count = 0;

    aiAnimation *animation = 0;

    DirectX::XMMATRIX scaling = DirectX::XMMatrixIdentity();
    DirectX::XMVECTOR orientation = DirectX::XMQuaternionIdentity();
    DirectX::XMFLOAT3 position = {};

    VertexShader::BufferData buffer_data;

    StdVector<Mesh<T>> meshes;

  private:
    Assimp::Importer importer;

    const aiScene *scene = 0;
    const aiTexture *ai_texture = 0;

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<decltype(clock)> animation_start_time = {};

    Animations animations;
};
