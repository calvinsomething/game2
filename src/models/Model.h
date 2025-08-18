#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <stdexcept>

#include "../Gfx/VertexShader.h"

class Model
{
  public:
    Model();

    Model(const char *file_name, std::vector<Vertex> &vertices, std::vector<uint32_t> &indices)
    {
        Assimp::Importer importer;

        scene = importer.ReadFile(file_name, aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                                 aiProcess_JoinIdenticalVertices | aiProcess_SortByPType);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            throw std::runtime_error(importer.GetErrorString());
        }

        if (!scene->HasMeshes())
        {
            throw std::runtime_error("Model loaded with no meshes.");
        }

        // Get first node with a mesh
        aiNode *node = scene->mRootNode;
        for (size_t i = 0; i < scene->mRootNode->mNumChildren; ++i)
        {
            if (!node->mNumMeshes)
            {
                node = node->mChildren[i];
                break;
            }
        }

        aiMesh &mesh = *scene->mMeshes[node->mMeshes[0]];

        start_vertex = vertices.size();

        for (size_t i = 0; i < mesh.mNumVertices; ++i)
        {
            aiVector3D &v = mesh.mVertices[i];

            vertices.push_back(
                {DirectX::XMFLOAT4(v.x, v.y, v.z - 18.0f, 1.0f), DirectX::XMFLOAT4(0.1f, 0.7f, 0.3f, 1.0f)});
        }

        vertex_count = vertices.size() - start_vertex;

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

        transform = DirectX::XMMatrixIdentity();
    }

    void bind();

    void update(const DirectX::XMMATRIX &xform);

    UINT get_index_count();
    UINT get_vertex_count();

    void set_position(float x, float y, float z);
    void translate(float x, float y, float z);

    DirectX::XMMATRIX get_transform();

  protected:
    UINT start_vertex, vertex_count, start_index, index_count;

    DirectX::XMMATRIX transform;

    DirectX::XMFLOAT3 position;

  private:
    const aiScene *scene = 0;

    VertexShader::BufferData buffer_data;
};
