#pragma once

#include "Gfx.h"

#include <wrl.h>

#include <vector>

class Shader : protected GfxAccess
{
  public:
    virtual void bind() = 0;

  protected:
    std::vector<char> load(const char *);

    Shader(Gfx &gfx);

    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
};
