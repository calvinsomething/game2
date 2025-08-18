#pragma once

#include "../Gfx/VertexShader.h"
#include "Model.h"

class Cube : public Model
{
  public:
    Cube(std::vector<Vertex> &vertices, std::vector<uint32_t> &indices);

    void bind();

    void update(const DirectX::XMMATRIX &xform);

    void set_position(float x, float y, float z);
    void translate(float x, float y, float z);

  private:
    VertexShader::BufferData buffer_data;
};
