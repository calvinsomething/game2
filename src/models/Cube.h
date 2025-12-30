#pragma once

#include "Model.h"

#include "../util.h"

class Cube : public Model<TextureVertex>
{
  public:
    Cube(Gfx &gfx, const std::wstring &texture_file_name, StdVector<TextureVertex> &vertices,
         StdVector<uint32_t> &indices, StdVector<Material> &materials, StdVector<Texture> &textures);

    void bind();

    void update(const DirectX::XMMATRIX &xform);

    void set_position(float x, float y, float z);
    void translate(float x, float y, float z);
};
