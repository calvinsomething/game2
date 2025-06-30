#pragma once

#include "Gfx.h"

#include <wrl.h>

#include <vector>

class Shader
{
    friend class Gfx;

  public:
    virtual void bind() = 0;

  protected:
    std::vector<char> load(const char *);

    Shader(Gfx &gfx);

    ID3D11Device *device = 0;
    ID3D11DeviceContext *ctx = 0;

    Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout;
};
