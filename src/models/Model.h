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
#include "Bone.h"
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
                                                 aiProcess_SortByPType | aiProcess_FlipUVs);

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

            for (size_t i = 0; i < scene->mAnimations[0]->mNumChannels; ++i)
            {
                node_animations.insert(
                    {scene->mAnimations[0]->mChannels[i]->mNodeName.C_Str(), scene->mAnimations[0]->mChannels[i]});
            }
        }

        start_vertex = vertices.size();
        start_index = indices.size();

        std::unordered_map<const aiNode *, size_t> bone_index_by_node;
        std::unordered_map<size_t, size_t> bone_parent_index_by_child_index;
        std::unordered_map<const aiNode *, std::vector<size_t>> bone_child_index_by_node;

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
                for (size_t i = 0; i < mesh.mNumBones; ++i)
                {
                    const aiNode *node = scene->mRootNode->findBoneNode(mesh.mBones[i]);

                    if (bone_index_by_node.find(node) == bone_index_by_node.end())
                    {
                        size_t bone_index = bones.size();
                        bones.push_back(Bone(mesh.mBones[i], node, vertices, start_vertex, bone_index));

                        bone_matrices.push_back(DirectX::XMMatrixIdentity());

                        bone_index_by_node.insert({node, bone_index});

                        auto p = bone_index_by_node.find(node->mParent);
                        if (p == bone_index_by_node.end())
                        {
                            auto c = bone_child_index_by_node.find(node->mParent);

                            if (c == bone_child_index_by_node.end())
                            {
                                if (node->mParent)
                                {
                                    bone_child_index_by_node.insert({node->mParent, {bone_index}});
                                }
                            }
                            else
                            {
                                c->second.push_back(bone_index);
                            }
                        }
                        else
                        {
                            bone_parent_index_by_child_index.insert({bone_index, p->second});
                        }

                        // find this node's previously loaded children
                        auto c = bone_child_index_by_node.find(node);
                        if (c != bone_child_index_by_node.end())
                        {
                            for (size_t i : c->second)
                            {
                                bone_parent_index_by_child_index.insert({i, bone_index});
                            }
                        }
                    }
                }
            }
        }

        if (!bones.empty())
        {
            for (Bone &b : bones)
            {
                for (Bone *c : b.children)
                {
                    if (c->index > 260)
                    {
                        throw std::runtime_error("CHILD");
                    }
                }

                auto parent_index = bone_parent_index_by_child_index.find(b.index);

                if (parent_index != bone_parent_index_by_child_index.end())
                {
                    b.parent = &bones[parent_index->second];
                    b.parent->children.push_back(&b);
                }
                else
                {
                    const aiNode *node = b.node->mParent;

                    while (node)
                    {
                        std::cout << node->mName.C_Str() << "\n";
                        if (bone_index_by_node.find(node) == bone_index_by_node.end())
                        {
                            b.set_intermediate_transform(node);

                            node = node->mParent;
                        }
                    }
                }
            }

            root_bone = &bones[0];

            auto next = root_bone;
            while (next)
            {
                root_bone = next;
                next = next->parent;
            }

            inverse_global_transform = Bone::get_inverted_transform(scene->mRootNode);
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

                    meshes.emplace_back(mesh, vertices, start_vertex, indices, start_index, tc_index, &textures,
                                        texture_index);
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

        if (root_bone)
        {
            double animation_time_sec = ((clock.now() - animation_start_time).count()) * 0.000000001;

            double time_in_ticks = scene->mAnimations[0]->mTicksPerSecond * animation_time_sec;

            update_bone(root_bone, inverse_global_transform, time_in_ticks);
        }
    }

    void update_bone(Bone *bone, const DirectX::XMMATRIX &parent_transform, double time_in_ticks)
    {
        if (!bone)
        {
            return;
        }

        aiNodeAnim *na = 0;

        if (time_in_ticks >= 0 && time_in_ticks <= scene->mAnimations[0]->mDuration)
        {
            auto node_animation = node_animations.find(bone->get_node_name());
            if (node_animation != node_animations.end())
            {
                na = node_animation->second;
            }
        }
        else
        {
            animation_start_time = {};
        }

        auto &animated_matrix = bone_matrices[bone->index];

        auto global_transform = bone->animate(parent_transform, na, time_in_ticks, animated_matrix);

        animated_matrix = DirectX::XMMatrixTranspose(animated_matrix);

        for (Bone *c : bone->children)
        {
            update_bone(c, global_transform, time_in_ticks);
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

    std::vector<DirectX::XMMATRIX> bone_matrices;

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

    Bone *root_bone = 0;
    std::vector<Bone> bones;

    DirectX::XMMATRIX inverse_global_transform;

    std::unordered_map<std::string, size_t> texture_indices_by_file_name;

    std::chrono::high_resolution_clock clock;
    std::chrono::time_point<decltype(clock)> animation_start_time = {};

    std::unordered_map<std::string, aiAnimation *> animations;

    std::unordered_map<std::string, aiNodeAnim *> node_animations;
};
