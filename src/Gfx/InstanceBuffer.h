#pragma once

#include "Buffer.h"

#include <DirectXMath.h>

#include <cstring>
#include <stdexcept>

struct InstanceData
{
    DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();
    uint32_t bone_start;
};

class InstanceBuffer : public Buffer
{
  public:
    template <typename T, size_t N>
    InstanceBuffer(Gfx &gfx, T (*data)[N], UINT pitch = 0, UINT slice_pitch = 0)
        : Buffer(gfx, data, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE)
    {
        stride = sizeof(T);
    }

    void bind() override
    {
        throw std::runtime_error("InstanceBuffer does not implement 'bind'.");
    }
};
