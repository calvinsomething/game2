#include "Cube.h"

uint32_t indices[] = {0, 1, 2};
Vertex vertices[] = {{DirectX::XMFLOAT4(0.0f, 0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)},
                     {DirectX::XMFLOAT4(0.5f, -0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)},
                     {DirectX::XMFLOAT4(-0.5f, -0.5f, 0.0f, 1.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f)}};

std::optional<VertexBuffer> Cube::vb;
std::optional<VertexBuffer> Cube::instance_vb;
std::optional<IndexBuffer> Cube::ib;
std::optional<VertexShader> Cube::vs;
std::optional<PixelShader> Cube::ps;

Cube::Cube(Gfx &gfx) : gfx(gfx)
{
    if (!initialized)
    {
        initialized = true;

        vb.emplace(gfx, vertices);
        instance_vb.emplace(gfx,
                            model_transform); // TODO the data for the per-instance vertex buffers should be contiguous,
                                              // then I can update the vertex buffer for all instances at once
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

void Cube::draw()
{
    vs->set_model_transform({
        1, 0, 0, //
        0, 1, 0, //
        0, 0, 1  //
    });

    gfx.end_frame();
}
