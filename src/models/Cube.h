#pragma once

#include "Model.h"

#include "../gfx/InstanceBuffer.h"
#include "../util.h"

class Cube : public Model<TextureVertex>
{
  public:
    Cube(Gfx &gfx, const std::wstring &texture_file_name, StdVector<TextureVertex> &vertices,
         StdVector<uint32_t> &indices, StdVector<Material> &materials, StdVector<Texture2D> &textures,
         InstanceData &instance_data);
};
