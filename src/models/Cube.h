#pragma once

#include "../Gfx/VertexShader.h"
#include "Model.h"

class Cube : public Model<TextureVertex>
{
  public:
    Cube(std::vector<TextureVertex> &vertices, std::vector<uint32_t> &indices);

    void bind();

    void update(const DirectX::XMMATRIX &xform);

    void set_position(float x, float y, float z);
    void translate(float x, float y, float z);
};
