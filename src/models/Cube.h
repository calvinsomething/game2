#pragma once

#include <optional>

#include "../Gfx/IndexBuffer.h"
#include "../Gfx/PixelShader.h"
#include "../Gfx/VertexBuffer.h"
#include "../Gfx/VertexShader.h"

class Cube
{
  public:
    Cube(Gfx &gfx);

    void bind();

    void draw();

  private:
    static std::optional<VertexBuffer> vb;
    static std::optional<VertexBuffer> instance_vb;
    static std::optional<IndexBuffer> ib;
    static std::optional<VertexShader> vs;
    static std::optional<PixelShader> ps;

    DirectX::XMFLOAT3X3 *model_transform;

    Gfx &gfx;

    bool initialized = false;
};
