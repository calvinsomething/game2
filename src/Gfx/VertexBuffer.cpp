#include "VertexBuffer.h"

void VertexBuffer::bind()
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;

    ctx->IASetVertexBuffers(0,        // the first input slot for binding
                            1,        // the number of buffers in the array
                            &buffer,  // the array of vertex buffers
                            &stride,  // array of stride values, one for each buffer
                            &offset); // array of offset values, one for each buffer
}
