#include "Skybox.h"

#include "../Error.h"
#include "../gfx/ComputeShader.h"

extern const size_t CUBE_POSITIONS_COUNT;
extern DirectX::XMFLOAT3 cube_positions[];

extern const size_t CUBE_INDICES_COUNT;
extern uint32_t cube_indices[];

constexpr size_t SKYBOX_SPAN = 1024;

struct Pixel
{
    unsigned char r, g, b, a;
};

Skybox::Skybox(Gfx &gfx, const char *texture_file_name)
    : GfxAccess(gfx), vs(gfx), ps(gfx), vb(gfx, cube_positions, CUBE_POSITIONS_COUNT),
      ib(gfx, cube_indices, CUBE_INDICES_COUNT)
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

    HANDLE_GFX_INFO(ctx->DrawIndexed(CUBE_INDICES_COUNT, 0, 0));
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
