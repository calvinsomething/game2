#include "Mesh.h"

template class Mesh<Vertex>;
template class Mesh<TextureVertex>;

template <> void Mesh<Vertex>::load_vertex(aiMesh &mesh, size_t i, std::vector<Vertex> &vertices)
{
    aiVector3D &v = mesh.mVertices[i];

    vertices.push_back({DirectX::XMFLOAT4(v.x, 1.0f - v.y, v.z, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)});
}

template <> void Mesh<TextureVertex>::load_vertex(aiMesh &mesh, size_t i, std::vector<TextureVertex> &vertices)
{
    aiVector3D &v = mesh.mVertices[i];

    if (texture_index != 99)
    {
        aiVector3D &tc = mesh.mTextureCoords[diffuse_tc_index][i];

        vertices.push_back({DirectX::XMFLOAT4(v.x, 1.0f - v.y, v.z, 1.0f), DirectX::XMFLOAT2(tc.x, tc.y)});
    }
    else
    {
        vertices.push_back({DirectX::XMFLOAT4(v.x, 1.0f - v.y, v.z, 1.0f), DirectX::XMFLOAT2(0, 0)});
    }
}
