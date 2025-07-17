#pragma once

#include "../Gfx/VertexShader.h"

class Cube
{
  public:
    Cube(Gfx &gfx);

    void bind();

    void update();

  private:
    Gfx &gfx;

    bool initialized = false;

    VertexShader::BufferData buffer_data;
};
