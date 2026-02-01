#include "Skybox.h"

#include "../Error.h"

extern const size_t CUBE_POSITIONS_COUNT;
extern DirectX::XMFLOAT3 cube_positions[];

extern const size_t CUBE_INDICES_COUNT;
extern uint32_t cube_indices[];

Skybox::Skybox(Gfx &gfx, const char *texture_file_name)
    : GfxAccess(gfx), vs(gfx), ps(gfx), vb(gfx, cube_positions, CUBE_POSITIONS_COUNT),
      ib(gfx, cube_indices, CUBE_INDICES_COUNT)
{
    ps.load_texture(texture_file_name);
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
