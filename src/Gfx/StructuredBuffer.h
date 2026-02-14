#pragma once

#include "Buffer.h"

#include "../Error.h"
#include "../util.h"
#include "Gfx.h"

class StructuredBuffer : public Buffer
{
  public:
    template <typename T>
    StructuredBuffer(Gfx &gfx, size_t slot_index, StdVector<T> &elements)
        : Buffer(gfx), stride(sizeof(T)), slot_index(slot_index)
    {
        init(elements.data(), elements.size());
    }

    StructuredBuffer(Gfx &gfx, size_t slot_index, size_t element_count, size_t element_size)
        : Buffer(gfx), stride(element_size), slot_index(slot_index)
    {
        init(nullptr, element_count);
    }

    void init(void *data, size_t element_count, D3D11_USAGE usage = D3D11_USAGE_DYNAMIC,
              UINT cpu_access = D3D11_CPU_ACCESS_WRITE)
    {
        Buffer::init(data, element_count * stride, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, cpu_access, 0, 0,
                     stride, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);

        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = element_count;

        HANDLE_GFX_ERR(device->CreateShaderResourceView(buffer.Get(), &desc, srv.GetAddressOf()));
    }

    void bind() override
    {
        HANDLE_GFX_INFO(ctx->VSSetShaderResources(slot_index, 1, srv.GetAddressOf()));
    }

    void bind_ps()
    {
        HANDLE_GFX_INFO(ctx->PSSetShaderResources(slot_index, 1, srv.GetAddressOf()));
    }

    void bind_cs()
    {
        HANDLE_GFX_INFO(ctx->CSSetShaderResources(slot_index, 1, srv.GetAddressOf()));
    }

  private:
    size_t stride = 0, slot_index = 0;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
};
