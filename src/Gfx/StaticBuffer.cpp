#include "StaticBuffer.h"

#include "../Error.h"

StaticBuffer::StaticBuffer(Gfx &gfx, size_t slot_index) : Buffer(gfx), slot_index(slot_index)
{
}

StaticBuffer::StaticBuffer(Gfx &gfx, size_t slot_index, void *data, size_t element_count, size_t element_size,
                           DXGI_FORMAT format)
    : Buffer(gfx), stride(element_size), slot_index(slot_index)
{
    init(data, element_count, element_size, format);
}

void StaticBuffer::init(void *data, size_t element_count, size_t element_size, DXGI_FORMAT format)
{
    stride = element_size;

    Buffer::init(data, element_count * stride, D3D11_BIND_SHADER_RESOURCE, D3D11_USAGE_IMMUTABLE);

    D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.Format = format;
    desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;

    desc.Buffer.FirstElement = 0;
    desc.Buffer.NumElements = element_count;

    HANDLE_GFX_ERR(device->CreateShaderResourceView(buffer.Get(), &desc, srv.GetAddressOf()));
}

void StaticBuffer::bind()
{
    HANDLE_GFX_INFO(ctx->CSSetShaderResources(slot_index, 1, srv.GetAddressOf()));
}
