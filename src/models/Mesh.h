#pragma once

#include <DirectXMath.h>
#include <assimp/mesh.h>
#include <wrl.h>

#include <stdexcept>

#include "../gfx/VertexShader.h"
#include "Bone.h"
#include "Material.h"

class MeshBase
{
  public:
    UINT get_index_count() const
    {
        return UINT(index_count);
    }

    UINT get_vertex_count() const
    {
        return UINT(vertex_count);
    }

  protected:
    MeshBase()
    {
    }

    size_t start_vertex, vertex_count, start_index, index_count;
};

template <typename T> class Mesh : public MeshBase
{
  public:
    Mesh(aiMesh &mesh, StdVector<T> &vertices, StdVector<uint32_t> &indices, Material material,
         TextureCoordinateIndices coordinate_indices)
        : vertices(vertices), indices(indices), material(material)
    {
        load_vertices(mesh, vertices, coordinate_indices);

        load_indices(mesh, indices);

        name = mesh.mName.C_Str();
    }

    void animate(const StdUnorderedMap<const aiNode *, const aiNodeAnim *> &node_animations, double time_in_ticks)
    {
        if (root_bone)
        {
            update_bone(root_bone, node_animations, DirectX::XMMatrixIdentity(), time_in_ticks);
        }
    }

    void load_bones(aiMesh &mesh, StdVector<T> &vertices)
    {
        StdUnorderedMap<const aiNode *, size_t> bone_index_by_node;
        StdUnorderedMap<size_t, size_t> bone_parent_index_by_child_index;
        StdUnorderedMap<const aiNode *, StdVector<size_t>> bone_child_index_by_node;

        for (size_t i = 0; i < mesh.mNumBones; ++i)
        {
            aiNode *node = mesh.mBones[i]->mNode;

            if (bone_index_by_node.find(node) == bone_index_by_node.end())
            {
                size_t bone_index = bones.size();
                bones.push_back(
                    Bone(mesh.mBones[i], node, vertices, start_vertex, bone_index, bone_data_by_vertex_index));

                for (auto &p : bone_data_by_vertex_index)
                {
                    sort_bones_by_weight(vertices[p.first], p.second);
                }

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

        if (!bones.empty())
        {
            for (Bone &b : bones)
            {
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
        }
    }

    StdVector<DirectX::XMMATRIX> bone_matrices;

    Material get_material()
    {
        return material;
    }

  private:
    StdVector<T> &vertices;
    StdVector<uint32_t> &indices;

    Material material;

    // helpers
    void load_vertices(aiMesh &mesh, StdVector<T> &vertices, TextureCoordinateIndices coordinate_indices)
    {
        start_vertex = vertices.size();

        vertices.reserve(vertices.size() + mesh.mNumVertices);

        for (size_t i = 0; i < mesh.mNumVertices; ++i)
        {
            load_vertex(mesh, i, vertices, coordinate_indices);
        }

        vertex_count = vertices.size() - start_vertex;
    }

    void update_bone(Bone *bone, const StdUnorderedMap<const aiNode *, const aiNodeAnim *> &node_animations,
                     const DirectX::XMMATRIX &parent_transform, double time_in_ticks)
    {
        if (!bone)
        {
            return;
        }

        const aiNodeAnim *na = 0;

        auto node_animation = node_animations.find(bone->node);
        if (node_animation != node_animations.end())
        {
            na = node_animation->second;

            auto &animated_matrix = bone_matrices[bone->index];

            auto global_transform = bone->animate(parent_transform, na, time_in_ticks, animated_matrix);

            animated_matrix = DirectX::XMMatrixTranspose(animated_matrix);

            for (Bone *c : bone->children)
            {
                update_bone(c, node_animations, global_transform, time_in_ticks);
            }
        }
    }

    void load_indices(aiMesh &mesh, StdVector<uint32_t> &indices)
    {
        start_index = indices.size();

        for (size_t i = 0; i < mesh.mNumFaces; ++i)
        {
            aiFace &f = mesh.mFaces[i];

#ifndef NDEBUG
            if (f.mNumIndices != 3)
            {
                throw std::runtime_error("Model faces must have 3 indices.");
            }
#endif

            indices.push_back(f.mIndices[0]);
            indices.push_back(f.mIndices[1]);
            indices.push_back(f.mIndices[2]);
        }

        index_count = indices.size() - start_index;
    }

    void load_vertex(aiMesh &mesh, size_t i, StdVector<T> &vertices, TextureCoordinateIndices coordinate_indices)
    {
        static_assert(0, "Unimplemented specialization of Mesh::load_vertex.");
    }

    Bone *root_bone = 0;
    StdVector<Bone> bones;
    std::string name;

    StdUnorderedMap<unsigned, StdVector<BoneData>> bone_data_by_vertex_index;
};

template <>
void Mesh<Vertex>::load_vertex(aiMesh &mesh, size_t i, StdVector<Vertex> &vertices,
                               TextureCoordinateIndices coordinate_indices);
template <>
void Mesh<TextureVertex>::load_vertex(aiMesh &mesh, size_t i, StdVector<TextureVertex> &vertices,
                                      TextureCoordinateIndices coordinate_indices);

template <> void Mesh<TextureVertex>::load_bones(aiMesh &mesh, StdVector<TextureVertex> &vertices);

template <typename T> void sort_bones_by_weight(T &vertex, StdVector<BoneData> &bone_data)
{
    for (unsigned i = 0; i < vertex.bone.count; ++i)
    {
        bone_data.push_back(BoneData{vertex.bone.indices[i], vertex.bone.weights[i]});
    }

    std::sort(bone_data.begin(), bone_data.end(), [](BoneData a, BoneData b) { return a.weight > b.weight; });

    for (unsigned i = 0; i < vertex.bone.count; ++i)
    {
        vertex.bone.indices[i] = bone_data[i].index;
        vertex.bone.weights[i] = bone_data[i].weight;
    }
}
