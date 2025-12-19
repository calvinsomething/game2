#pragma once

#include "Buffer.h"

#include "../Error.h"
#include "../util.h"
#include "Gfx.h"

class StructuredBuffer : public Buffer
{
  public:
    template <typename T> StructuredBuffer(Gfx &gfx, StdVector<T> &elements) : Buffer(gfx)
    {
        stride = sizeof(T);

        init(elements.data(), elements.size() * stride, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC,
             D3D11_CPU_ACCESS_WRITE, 0, 0, stride, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);

        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = elements.size();

        HANDLE_GFX_ERR(device->CreateShaderResourceView(buffer.Get(), &desc, srv.GetAddressOf()));
    }

    StructuredBuffer(Gfx &gfx, size_t element_count, size_t element_size) : Buffer(gfx)
    {
        stride = element_size;

        init(nullptr, element_count * stride, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE,
             0, 0, stride, D3D11_RESOURCE_MISC_BUFFER_STRUCTURED);

        D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
        desc.Format = DXGI_FORMAT_UNKNOWN;
        desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

        desc.Buffer.FirstElement = 0;
        desc.Buffer.NumElements = element_count;

        HANDLE_GFX_ERR(device->CreateShaderResourceView(buffer.Get(), &desc, srv.GetAddressOf()));
    }

    void bind() override
    {
        HANDLE_GFX_INFO(ctx->VSSetShaderResources(0, 1, srv.GetAddressOf()));
    }

  private:
    size_t stride;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
};
