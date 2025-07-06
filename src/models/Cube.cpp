#include "Cube.h"

constexpr DirectX::XMFLOAT3X3 identity = {
    1, 0, 0, //
    0, 1, 0, //
    0, 0, 1, //
};

uint32_t indices[] = {0, 1, 2};
Vertex vertices[] = {{DirectX::XMFLOAT4(0.0f, 0.5f, 3.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
                     {DirectX::XMFLOAT4(0.5f, -0.5f, 3.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
                     {DirectX::XMFLOAT4(-0.5f, -0.5f, 3.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}};
// Vertex vertices[] = {{DirectX::XMFLOAT4(0.0f, 10.0f, 15.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
//                      {DirectX::XMFLOAT4(5.0f, 0.0f, 15.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
//                      {DirectX::XMFLOAT4(-5.5f, 0.0f, 15.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}};

std::optional<VertexBuffer> Cube::vb;
std::optional<IndexBuffer> Cube::ib;
std::optional<VertexShader> Cube::vs;
std::optional<PixelShader> Cube::ps;

Cube::Cube(Gfx &gfx) : gfx(gfx), model_xform(identity), world_xform(identity)
{
    if (!initialized)
    {
        initialized = true;

        vb.emplace(gfx, vertices);
        ib.emplace(gfx, indices);
        vs.emplace(gfx);
        ps.emplace(gfx);
    }
}

void Cube::bind()
{
    vb.value().bind();
    ib.value().bind();
    vs.value().bind();
    ps.value().bind();
}

void Cube::update()
{
    vs->set_transforms(
        {
            1, 0, 0, //
            0, 1, 0, //
            0, 0, 1  //
        },
        {
            0.9f, 0, 0, //
            0, 0.9f, 0, //
            0, 0, 1     //
        });
}
