#include "Shader.h"

#include "../util.h"

constexpr char SHADER_DIR[] = SHADERS_BIN;

const char *source_files[] = {"vs.cso",     "vs_tex.cso",    "vs_skybox.cso", "ps.cso",
                              "ps_tex.cso", "ps_skybox.cso", "cs_skybox.cso"};

Shader::Shader(Gfx &gfx) : GfxAccess(gfx)
{
}

std::string Shader::load(ShaderSource source)
{
    const char *file_name = source_files[source];

    std::string path;
    path.reserve(sizeof(SHADER_DIR) + 1 + strlen(file_name));
    path += SHADER_DIR;
    path += '/';
    path += file_name;

    std::string byte_code = load_file(path);

    return byte_code;
}
