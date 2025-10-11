#pragma once

#include <DirectXMath.h>
#include <assimp/mesh.h>
#include <wrl.h>

#include <stdexcept>
#include <vector>

#include "../gfx/Texture.h"
#include "../gfx/VertexShader.h"
#include "Bone.h"

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
    MeshBase(size_t model_start_vertex, size_t model_start_index)
        : model_start_vertex(model_start_vertex), model_start_index(model_start_index)
    {
    }

    size_t model_start_vertex, model_start_index, start_vertex, vertex_count, start_index, index_count;
};

template <typename T> class Mesh : public MeshBase
{
  public:
    Mesh(aiMesh &mesh, std::vector<T> &vertices, size_t model_start_vertex, std::vector<uint32_t> &indices,
         size_t model_start_index, size_t diffuse_tc_index, std::vector<Texture> *textures, size_t texture_index)
        : MeshBase(model_start_vertex, model_start_index), vertices(vertices), indices(indices),
          diffuse_tc_index(diffuse_tc_index), textures(textures), texture_index(texture_index)
    {
        assert(textures && !textures->empty() && "Mesh<TextureVertex> constructed without textures");

        load_vertices(mesh, vertices);

        load_indices(mesh, indices);
    }

    Mesh(aiMesh &mesh, std::vector<T> &vertices, size_t model_start_vertex, std::vector<uint32_t> &indices,
         size_t model_start_index)
        : MeshBase(model_start_vertex, model_start_index), vertices(vertices), indices(indices)
    {
        load_vertices(mesh, vertices);

        load_indices(mesh, indices);
    }

    Texture *get_texture()
    {
        throw std::runtime_error("Unimplemented specialization of Mesh::get_texture.");
    }

  private:
    std::vector<T> &vertices;
    std::vector<uint32_t> &indices;

    size_t diffuse_tc_index, texture_index;

    Bone *root_bone = 0;

    std::vector<Texture> *textures = 0;

    // helpers
    void load_vertices(aiMesh &mesh, std::vector<T> &vertices)
    {
        size_t start_vertex = vertices.size();

        vertices.reserve(vertices.size() + mesh.mNumVertices);

        for (size_t i = 0; i < mesh.mNumVertices; ++i)
        {
            load_vertex(mesh, i, vertices);
        }

        vertex_count = vertices.size() - start_vertex;
    }

    void load_indices(aiMesh &mesh, std::vector<uint32_t> &indices)
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

    void load_vertex(aiMesh &mesh, size_t i, std::vector<T> &vertices)
    {
        throw std::runtime_error("Unimplemented specialization of Mesh::load_vertex.");
    }
};

template <> void Mesh<Vertex>::load_vertex(aiMesh &mesh, size_t i, std::vector<Vertex> &vertices);

template <> void Mesh<TextureVertex>::load_vertex(aiMesh &mesh, size_t i, std::vector<TextureVertex> &vertices);

template <> Texture *Mesh<TextureVertex>::get_texture();
