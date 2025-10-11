#pragma once

#include "assimp/matrix4x4.h"
#include "assimp/mesh.h"
#include <DirectXMath.h>
#include <assimp/anim.h>
#include <assimp/scene.h>
#include <stdexcept>

#include "../Gfx/VertexShader.h"

class Bone
{
  public:
    Bone(aiBone *ai_bone, const aiNode *node, std::vector<TextureVertex> &vertices, size_t start_vertex, size_t index)
        : index(index), ai_bone(ai_bone), node(node)
    {
        assert(node && "Bone constructed without node.");

        float *m = &ai_bone->mOffsetMatrix.a1;
        offset = {
            m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15],
        };

        m = const_cast<float *>(&node->mTransformation.a1);
        bind_pose_transform = {
            m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11], m[12], m[13], m[14], m[15],
        };

        for (size_t i = 0; i < ai_bone->mNumWeights; ++i)
        {
            size_t j = ai_bone->mWeights[i].mVertexId + start_vertex;

            if (j >= vertices.size())
            {
                throw std::runtime_error("calculated vertex index exceeds size of vertices array");
            }

            TextureVertex &v = vertices[j];

            if (v.bone_count == 4)
            {
                break;
                // throw std::runtime_error("vertex has more than 4 bones");
            }

            v.bone_indices[v.bone_count] = index;
            v.bone_weights[v.bone_count] = ai_bone->mWeights[i].mWeight;

            ++v.bone_count;
        }
    }

    void animate(const DirectX::XMMATRIX &parent_transform, DirectX::XMMATRIX &animated_matrix, unsigned ms)
    {
        // offset moves vertex to bone space
        // mTransform moves vertex from current bone space to parent's bone space

        // TODO find animated mTransform if exists, otherwise use bind_pose_transform
        animated_matrix = DirectX::XMMatrixMultiply(bind_pose_transform, parent_transform);
    }

    Bone *parent = 0;

    std::vector<Bone *> children;

    size_t index;

  private:
    aiBone *ai_bone = 0;
    const aiNode *node = 0;

    DirectX::XMMATRIX offset, bind_pose_transform;
};
