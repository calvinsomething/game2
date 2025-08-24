#include "Cube.h"

#include "../util.h"

// Vertex vertices[] = {{DirectX::XMFLOAT4(-5.0f, 5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
//                      {DirectX::XMFLOAT4(5.0f, 5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
//                      {DirectX::XMFLOAT4(5.0f, -5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
//                      {DirectX::XMFLOAT4(-5.0f, -5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
//                      {DirectX::XMFLOAT4(-5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)},
//                      {DirectX::XMFLOAT4(5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)},
//                      {DirectX::XMFLOAT4(5.0f, -5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
//                      {DirectX::XMFLOAT4(-5.0f, -5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)}};

TextureVertex vertices[] = {{DirectX::XMFLOAT4(-5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT2(0.33f, 0.0f)},
                            {DirectX::XMFLOAT4(5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT2(0.67f, 0.0f)},
                            {DirectX::XMFLOAT4(-5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.25f)},
                            {DirectX::XMFLOAT4(-5.0f, 5.0f, -5.0f, 1.0f), DirectX::XMFLOAT2(0.33f, 0.25f)},
                            {DirectX::XMFLOAT4(5.0f, 5.0f, -5.0f, 1.0f), DirectX::XMFLOAT2(0.67f, 0.25f)},
                            {DirectX::XMFLOAT4(5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.25f)},
                            {DirectX::XMFLOAT4(-5.0f, -5.0f, 5.0f, 1.0f), DirectX::XMFLOAT2(0.0f, 0.5f)},
                            {DirectX::XMFLOAT4(-5.0f, -5.0f, -5.0f, 1.0f), DirectX::XMFLOAT2(0.33f, 0.5f)},
                            {DirectX::XMFLOAT4(5.0f, -5.0f, -5.0f, 1.0f), DirectX::XMFLOAT2(0.67f, 0.5f)},
                            {DirectX::XMFLOAT4(5.0f, -5.0f, 5.0f, 1.0f), DirectX::XMFLOAT2(1.0f, 0.5f)},
                            {DirectX::XMFLOAT4(-5.0f, -5.0f, 5.0f, 1.0f), DirectX::XMFLOAT2(0.33f, 0.75f)},
                            {DirectX::XMFLOAT4(5.0f, -5.0f, 5.0f, 1.0f), DirectX::XMFLOAT2(0.67f, 0.75f)},
                            {DirectX::XMFLOAT4(-5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT2(0.33f, 1.0f)},
                            {DirectX::XMFLOAT4(5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT2(0.67f, 1.0f)}};

// uint32_t indices[] = {
//     0, 1, 2, 2, 3, 0, 0, 4, 5, 5, 1, 0, 4, 7, 6, 6, 5, 4, 4, 0, 3, 3, 7, 4, 3, 2, 6, 6, 7, 3, 1, 5, 6, 6, 2, 1,
// };

uint32_t indices[] = {0, 1, 4, 4, 3, 0, 2, 3, 7,  7,  6,  2, 3,  4,  8,  8,  7,  3,
                      4, 5, 9, 9, 8, 4, 7, 8, 11, 11, 10, 7, 10, 11, 13, 13, 12, 10};

Cube::Cube(std::vector<TextureVertex> &vertices, std::vector<uint32_t> &indices)
{
    transform = DirectX::XMMatrixIdentity();

    vertex_count = ARRAYSIZE(::vertices);
    start_vertex = load_vector(vertices, ::vertices, vertex_count);

    index_count = ARRAYSIZE(::indices);
    start_index = load_vector(indices, ::indices, index_count);
}

void Cube::bind()
{
}

void Cube::update(const DirectX::XMMATRIX &xform)
{
    transform *= xform;
}

void Cube::set_position(float x, float y, float z)
{
    position = DirectX::XMFLOAT3(x, y, z);
}

void Cube::translate(float x, float y, float z)
{
    position.x += x;
    position.y += y;
    position.z += z;
}
