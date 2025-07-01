#include "IndexBuffer.h"

void IndexBuffer::bind()
{
    ctx->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}
