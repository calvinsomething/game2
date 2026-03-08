#pragma once

#include <assimp/material.h>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/Importer.hpp>
#include <chrono>
#include <cstring>
#include <stdexcept>

#include "../gfx/InstanceBuffer.h"
#include "../gfx/StructuredBuffer.h"
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

        start_vertex = vertices.size();
        start_index = indices.size();

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

        vertex_count = vertices.size() - start_vertex;
    }

    void loop_animation(const std::string &name)
    {
        start_animation(name);

        animations.set_looping(true);
    }

    void start_animation(const std::string &name, std::chrono::milliseconds delay = std::chrono::milliseconds(0))
    {
        animations.set_animation(name);

        animation_start_time = clock.now() + delay;
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
            double animation_time_sec = std::chrono::duration<double>(clock.now() - animation_start_time).count();

            double time_in_ticks = animation->ticks_per_second * animation_time_sec;

            if (time_in_ticks > animation->duration && animations.is_looping())
            {
                animation_start_time = clock.now();
                time_in_ticks = 0;
            }

            if (time_in_ticks <= animation->duration)
            {
                if (time_in_ticks >= 0)
                {
                    for (auto &m : meshes)
                    {
                        m.animate(animation->node_anim_map, time_in_ticks);
                    }
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

    UINT get_start_vertex()
    {
        return start_vertex;
    }

    UINT get_vertex_count()
    {
        return vertex_count;
    }

    UINT get_start_index()
    {
        return start_index;
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

    DirectX::XMFLOAT3 get_position() const
    {
        return position;
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

    // y_origin_by_height_ratio must be between 0.0f - 1.0f, otherwise it will be ignored.
    // Sets the Y origin relative to the model's height (0.0f == base of model, 1.0f == top of model)
    void set_correction_transform(const DirectX::XMMATRIX &xform, float y_origin_by_height_ratio = -1.0f,
                                  const StdVector<T> &vertices = {})
    {
        float min_y = std::numeric_limits<float>::infinity(), max_y = std::numeric_limits<float>::lowest();

        correction_transform = xform;

        if (y_origin_by_height_ratio >= 0.0f && y_origin_by_height_ratio <= 1.0f && !vertices.empty())
        {
            for (size_t i = 0; i < vertex_count; ++i)
            {
                DirectX::XMFLOAT3 f3 = transform(vertices[start_vertex + i].position, correction_transform);

                min_y = f3.y < min_y ? f3.y : min_y;
                max_y = f3.y > max_y ? f3.y : max_y;
            }
            float model_height = max_y - min_y;

            correction_transform = DirectX::XMMatrixMultiply(
                correction_transform,
                DirectX::XMMatrixTranslation(0.0f, -min_y - y_origin_by_height_ratio * model_height, 0.0f));
        }
    }

    template <typename S> void draw_indexed_instanced(S &shader, size_t &index, size_t &vertex, size_t instance_index)
    {
        for (Mesh<T> &m : meshes)
        {
            UINT n = m.get_index_count();

            m.constant_buffer.bind();

            // Blender apparently doesn't export the two_sided fbx mesh property...
            // gfx.set_rasterizer_state(m.is_two_sided() ? Gfx::RasterizerState::TWO_SIDED
            //                                           : Gfx::RasterizerState::STANDARD);
            shader.draw_indexed_instanced(index, n, instance_index, 1, vertex);

            index += n;
            vertex += m.get_vertex_count();
        }
    }

    size_t update_bone_buffer(StructuredBuffer &bone_data_buffer, size_t start_offset)
    {
        size_t offset = start_offset;

        for (Mesh<T> &m : meshes)
        {
            size_t animation_data_size = m.bone_matrices.size() * sizeof(m.bone_matrices[0]);
            bone_data_buffer.update(m.bone_matrices.data(), animation_data_size, offset);
            offset += animation_data_size;
        }

        return offset;
    }

    DirectX::XMMATRIX get_transform()
    {
        return correction_transform * scaling * DirectX::XMMatrixRotationQuaternion(orientation) *
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
    DirectX::XMMATRIX correction_transform = DirectX::XMMatrixIdentity();

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

    size_t start_vertex = 0, vertex_count = 0, start_index = 0;
};
