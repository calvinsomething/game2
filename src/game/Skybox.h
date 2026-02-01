#pragma once

#include "../gfx/IndexBuffer.h"
#include "../gfx/PixelShader.h"
#include "../gfx/VertexBuffer.h"
#include "../gfx/VertexShader.h"

class Skybox : private GfxAccess
{
  public:
    Skybox(Gfx &gfx, const char *texture_file_name);

    void bind_and_draw();

  private:
    SkyboxVertexShader vs;
    SkyboxPixelShader ps;
    VertexBuffer vb;
    IndexBuffer ib;
};
