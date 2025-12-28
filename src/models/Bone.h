#pragma once

#include "assimp/matrix4x4.h"
#include "assimp/mesh.h"
#include <DirectXMath.h>
#include <assimp/anim.h>
#include <assimp/scene.h>
#include <stdexcept>

#include "../util.h"

inline DirectX::XMMATRIX get_z_up_matrix(float *m)
{
    return DirectX::XMMATRIX{
        m[0], m[8], m[4], m[12], m[2], m[10], m[6], m[14], m[1], m[9], m[5], m[13], m[3], m[11], m[7], m[15],
    };
}

struct BoneData
{
    unsigned index;
    float weight;
};

class Bone
{
  public:
    template <typename T>
    Bone(aiBone *ai_bone, const aiNode *node, StdVector<T> &vertices, size_t start_vertex, size_t index,
         StdUnorderedMap<unsigned, StdVector<BoneData>> &bone_data_by_vertex_index)
        : index(index), node(node), intermediate_transform(DirectX::XMMatrixIdentity())
    {
        assert(node && "Bone constructed without node.");

        for (size_t i = 0; i < ai_bone->mNumWeights; ++i)
        {
            size_t j = ai_bone->mWeights[i].mVertexId + start_vertex;

            if (j >= vertices.size())
            {
                throw std::runtime_error("calculated vertex index exceeds size of vertices array");
            }

            T &v = vertices[j];

            if (v.bone.count < 4)
            {
                v.bone.indices[v.bone.count] = index;
                v.bone.weights[v.bone.count] = ai_bone->mWeights[i].mWeight;

                ++v.bone.count;
            }
            else
            {
                auto existing = bone_data_by_vertex_index.find(j);

                BoneData bd = BoneData{static_cast<unsigned>(index), ai_bone->mWeights[i].mWeight};

                if (existing == bone_data_by_vertex_index.end())
                {
                    bone_data_by_vertex_index.insert({j, StdVector<BoneData>{bd}});
                }
                else
                {
                    existing->second.push_back(bd);
                }
            }
        }

        float *m = &ai_bone->mOffsetMatrix.a1;
        offset = get_z_up_matrix(m);

        m = const_cast<float *>(&node->mTransformation.a1);

        bind_pose_transform = get_z_up_matrix(m);
    }

    DirectX::XMMATRIX get_animation_scaling(const aiNodeAnim *node_animation, double time_in_ticks)
    {
        for (size_t i = 0; i < node_animation->mNumScalingKeys; ++i)
        {
            if (node_animation->mScalingKeys[i].mTime >= time_in_ticks)
            {
                auto &s = node_animation->mScalingKeys[i].mValue;

                return DirectX::XMMatrixScaling(s.x, s.z, s.y);
            }
        }

        return DirectX::XMMatrixIdentity();
    }

    DirectX::XMMATRIX get_animation_rotation(const aiNodeAnim *node_animation, double time_in_ticks)
    {
        for (size_t i = 0; i < node_animation->mNumRotationKeys; ++i)
        {
            if (node_animation->mRotationKeys[i].mTime >= time_in_ticks)
            {
                auto r = node_animation->mRotationKeys[i].mValue.GetMatrix();

                return DirectX::XMMATRIX{r.a1, r.c1, r.b1, 0.0f, //
                                         r.a3, r.c3, r.b3, 0.0f, //
                                         r.a2, r.c2, r.b2, 0.0f, //
                                         0.0f, 0.0f, 0.0f, 1.0f};
            }
        }

        return DirectX::XMMatrixIdentity();
    }

    DirectX::XMMATRIX get_animation_translation(const aiNodeAnim *node_animation, double time_in_ticks)
    {
        for (size_t i = 0; i < node_animation->mNumPositionKeys; ++i)
        {
            if (node_animation->mPositionKeys[i].mTime >= time_in_ticks)
            {
                auto &t = node_animation->mPositionKeys[i].mValue;

                return DirectX::XMMatrixTranslation(t.x, t.z, t.y);
            }
        }

        return DirectX::XMMatrixIdentity();
    }

    DirectX::XMMATRIX animate(const DirectX::XMMATRIX &parent_transform, const aiNodeAnim *node_animation,
                              double time_in_ticks, DirectX::XMMATRIX &animated_matrix)
    {
        DirectX::XMMATRIX global_transform;

        if (node_animation)
        {
            DirectX::XMMATRIX scaling = get_animation_scaling(node_animation, time_in_ticks);

            DirectX::XMMATRIX rotation = get_animation_rotation(node_animation, time_in_ticks);

            DirectX::XMMATRIX translation = get_animation_translation(node_animation, time_in_ticks);

            global_transform = scaling * rotation * translation * intermediate_transform * parent_transform;
        }
        else
        {
            global_transform = bind_pose_transform * parent_transform;
        }

        animated_matrix = offset * global_transform;

        return global_transform;
    }

    Bone *parent = 0;

    StdVector<Bone *> children;

    size_t index;

    const char *get_node_name()
    {
        return node->mName.C_Str();
    }

    void set_intermediate_transform(const aiNode *node)
    {
        aiVector3D scale, rotation, translation;

        node->mTransformation.Decompose(scale, rotation, translation);

        float tx = translation.x, ty = translation.y, tz = translation.z;

        if (scale.x)
        {
            tx /= scale.x;
        }
        if (scale.y)
        {
            ty /= scale.y;
        }
        if (scale.z)
        {
            tz /= scale.z;
        }

        auto rotation_xform = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.z, rotation.y);

        // apply armature rotation, reverse the translation, rotate back
        DirectX::XMMATRIX matrix = rotation_xform * DirectX::XMMatrixTranslation(-tx, -tz, -ty) *
                                   DirectX::XMMatrixInverse(nullptr, rotation_xform);

        intermediate_transform *= matrix;

        bind_pose_transform *= matrix;
    }

    const aiNode *node = 0;

  private:
    DirectX::XMMATRIX offset, bind_pose_transform, intermediate_transform;
};
