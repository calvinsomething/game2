#include "Cube.h"

Vertex vertices[] = {{DirectX::XMFLOAT4(-5.0f, 5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
                     {DirectX::XMFLOAT4(5.0f, 5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
                     {DirectX::XMFLOAT4(5.0f, -5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)},
                     {DirectX::XMFLOAT4(-5.0f, -5.0f, -5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f)},
                     {DirectX::XMFLOAT4(-5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f)},
                     {DirectX::XMFLOAT4(5.0f, 5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f)},
                     {DirectX::XMFLOAT4(5.0f, -5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f)},
                     {DirectX::XMFLOAT4(-5.0f, -5.0f, 5.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)}};

uint32_t indices[] = {0, 1, 2, 2, 3, 0, 0, 4, 5, 5, 1, 0, 4, 7, 6, 6, 5, 4,
                      4, 0, 3, 3, 7, 4, 3, 2, 6, 6, 7, 3, 1, 5, 6, 6, 2, 1};

std::optional<VertexBuffer> Cube::vb;
std::optional<IndexBuffer> Cube::ib;
std::optional<VertexShader> Cube::vs;
std::optional<PixelShader> Cube::ps;

Cube::Cube(Gfx &gfx)
    : gfx(gfx), buffer_data{{DirectX::XMMatrixIdentity(), DirectX::XMMATRIX{
                                                              1, 0, 0, 0,  //
                                                              0, 1, 0, 0,  //
                                                              0, 0, 1, 20, //
                                                              0, 0, 0, 1,  //
                                                          }}}
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
    buffer_data.transforms.model *= DirectX::XMMatrixRotationRollPitchYaw(0.005f, 0.002f, 0.003f);

    vs->set_buffer(&buffer_data);

    vs->draw_indexed(ARRAYSIZE(indices));
}
