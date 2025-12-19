#pragma once

#include "Model.h"

#include "../util.h"

class Cube : public Model<TextureVertex>
{
  public:
    Cube(StdVector<TextureVertex> &vertices, StdVector<uint32_t> &indices);

    void bind();

    void update(const DirectX::XMMATRIX &xform);

    void set_position(float x, float y, float z);
    void translate(float x, float y, float z);
};
