#include "VertexBuffer.h"

void VertexBuffer::bind()
{
    ctx->IASetVertexBuffers(0,                     // the first input slot for binding
                            1,                     // the number of buffers in the array
                            buffer.GetAddressOf(), // the array of vertex buffers
                            &stride,               // array of stride values, one for each buffer
                            &offset);              // array of offset values, one for each buffer
}

void VertexBuffers::bind()
{
    ctx->IASetVertexBuffers(0, count, buffers, strides, offsets);
}
