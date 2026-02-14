#pragma once

#include "Buffer.h"

class StaticBuffer : private Buffer
{
  public:
    StaticBuffer(Gfx &gfx, size_t slot_index);
    StaticBuffer(Gfx &gfx, size_t slot_index, void *data, size_t element_count, size_t element_size,
                 DXGI_FORMAT format);

    void init(void *data, size_t element_count, size_t element_size, DXGI_FORMAT format);

    void bind() override;

  private:
    size_t stride = 0, slot_index;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
};
