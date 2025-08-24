#include "Model.h"

Model::Model() : transform(DirectX::XMMatrixIdentity())
{
}

DirectX::XMMATRIX Model::get_transform()
{
    return transform * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}

void Model::bind()
{
}

void Model::update(const DirectX::XMMATRIX &xform)
{
    transform *= xform;
}

UINT Model::get_index_count()
{
    return UINT(index_count);
}

UINT Model::get_vertex_count()
{
    return UINT(vertex_count);
}

void Model::set_position(float x, float y, float z)
{
    position = DirectX::XMFLOAT3(x, y, z);
}

void Model::translate(float x, float y, float z)
{
    position.x += x;
    position.y += y;
    position.z += z;
}
