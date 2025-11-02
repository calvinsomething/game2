#include "Mesh.h"

template class Mesh<Vertex>;
template class Mesh<TextureVertex>;

template <> void Mesh<Vertex>::load_vertex(aiMesh &mesh, size_t i, std::vector<Vertex> &vertices)
{
    aiVector3D &v = mesh.mVertices[i];

    vertices.push_back({DirectX::XMFLOAT4(v.x, v.z, v.y, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)});
}

template <> void Mesh<TextureVertex>::load_vertex(aiMesh &mesh, size_t i, std::vector<TextureVertex> &vertices)
{
    aiVector3D &v = mesh.mVertices[i];

    aiVector3D &tc = mesh.mTextureCoords[diffuse_tc_index][i];

    vertices.push_back({DirectX::XMFLOAT4(v.x, v.z, v.y, 1.0f), DirectX::XMFLOAT2(tc.x, tc.y)});
}

template <> Texture *Mesh<TextureVertex>::get_texture()
{
    return &textures->at(texture_index);
}

template <> void Mesh<TextureVertex>::load_bones(aiMesh &mesh, std::vector<TextureVertex> &vertices)
{
    std::unordered_map<const aiNode *, size_t> bone_index_by_node;
    std::unordered_map<size_t, size_t> bone_parent_index_by_child_index;
    std::unordered_map<const aiNode *, std::vector<size_t>> bone_child_index_by_node;

    for (size_t i = 0; i < mesh.mNumBones; ++i)
    {
        aiNode *node = mesh.mBones[i]->mNode;

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
