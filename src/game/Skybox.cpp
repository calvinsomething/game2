#include "Skybox.h"

#include <winnt.h>

#include "../Error.h"
#include "../gfx/ComputeShader.h"

extern DirectX::XMFLOAT3 cube_positions[8];

static uint32_t cube_indices[] = {
    0, 1, 2, 0, 2, 3, // front
    4, 5, 6, 4, 6, 7, // back
    0, 5, 6, 0, 6, 3, // left
    1, 4, 7, 1, 7, 2, // right
    0, 1, 4, 0, 4, 5, // top
    3, 2, 7, 3, 7, 6, // bottom
};

constexpr size_t SKYBOX_SPAN = 1024;

struct Pixel
{
    unsigned char r, g, b, a;
};

Skybox::Skybox(Gfx &gfx, const char *texture_file_name)
    : GfxAccess(gfx), vs(gfx), ps(gfx), vb(gfx, cube_positions, ARRAYSIZE(cube_positions)),
      ib(gfx, cube_indices, ARRAYSIZE(cube_indices))
{
    ComputeShader cs(gfx, Shader::ShaderSource::COMPUTE_SKYBOX);
    cs.bind();

    StaticBuffer input(gfx, ComputeShader::Slot::INPUT_BUFFER);

    load_hdr(input, texture_file_name);

    ps.load_texture(cs, SKYBOX_SPAN);
}

void Skybox::bind_and_draw()
{
    vs.bind();
    ps.bind();

    vb.bind();
    ib.bind();

    ctx->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    HANDLE_GFX_INFO(ctx->DrawIndexed(ARRAYSIZE(cube_indices), 0, 0));
}

// Load HDR
//
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_HDR
#include <nothings/stb/stb_image.h>

void Skybox::load_hdr(StaticBuffer &buffer, const char *file_name)
{
    int width, height,
        n; // n is how many color channels the file contains, even though the output will be forced to be 4
    unsigned char *data = stbi_load(file_name, &width, &height, &n, 4);

    if (!data)
    {
        const char *reason = stbi_failure_reason();
        if (reason)
        {
            auto msg = build_string("Failed to load file '", file_name, "': ", reason);
            throw std::runtime_error(msg);
        }

        auto msg = build_string("Failed to load file: ", file_name);
        throw std::runtime_error(msg);
    }

    buffer.init(data, width * height, sizeof(Pixel), DXGI_FORMAT_R8G8B8A8_UNORM);

    buffer.bind();
}
