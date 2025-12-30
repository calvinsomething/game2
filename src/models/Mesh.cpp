#include "Mesh.h"

// helpers
DirectX::XMFLOAT3 get_normal(aiMesh &mesh, size_t i)
{
    DirectX::XMFLOAT3 normal;

    if (mesh.mNormals)
    {
        aiVector3D n = mesh.mNormals[i];
        normal.x = n.x;
        normal.y = n.y;
        normal.z = n.z;
    }

    return normal;
}

template class Mesh<Vertex>;
template class Mesh<TextureVertex>;

template <>
void Mesh<Vertex>::load_vertex(aiMesh &mesh, size_t i, StdVector<Vertex> &vertices,
                               TextureCoordinateIndices coordinate_indices)
{
    aiVector3D &v = mesh.mVertices[i];

    vertices.push_back({DirectX::XMFLOAT3(v.x, v.z, v.y)/*, get_normal(mesh, i)*/});

    // DirectX::XMFLOAT4 color;

    // aiColor4D *c = mesh.mColors[0];
    // if (c)
    // {
    //     color = {c[i].r, c[i].g, c[i].b, c[i].a};
    // }
    // else
    // {
    //     color = {1.0f, 1.0f, 1.0f, 1.0f};
    // }
}

template <>
void Mesh<TextureVertex>::load_vertex(aiMesh &mesh, size_t i, StdVector<TextureVertex> &vertices,
                                      TextureCoordinateIndices coordinate_indices)
{
    aiVector3D &v = mesh.mVertices[i];

    TextureCoordinates tc = {};

    if (coordinate_indices.diffuse_coordinates_index != -1)
    {
        aiVector3D &dtc = mesh.mTextureCoords[coordinate_indices.diffuse_coordinates_index][i];
        tc.diffuse.x = dtc.x;
        tc.diffuse.y = dtc.y;
    }

    if (coordinate_indices.normal_coordinates_index != -1)
    {
        aiVector3D &ntc = mesh.mTextureCoords[coordinate_indices.normal_coordinates_index][i];
        tc.normal.x = ntc.x;
        tc.normal.y = ntc.y;
    }

    vertices.push_back({DirectX::XMFLOAT3(v.x, v.z, v.y)/*, get_normal(mesh, i)*/, tc});
}

template <> void Mesh<TextureVertex>::load_bones(aiMesh &mesh, StdVector<TextureVertex> &vertices)
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
            bones.push_back(Bone(mesh.mBones[i], node, vertices, start_vertex, bone_index, bone_data_by_vertex_index));

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
