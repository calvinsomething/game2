#include "Shader.h"

#include "../util.h"

constexpr char SHADER_DIR[] = SHADERS_BIN;

Shader::Shader(Gfx &gfx) : GfxAccess(gfx)
{
}

std::string Shader::load(const char *file_name)
{
    std::string path;
    path.reserve(sizeof(SHADER_DIR) + 1 + strlen(file_name));
    path += SHADER_DIR;
    path += '/';
    path += file_name;

    std::string byte_code = load_file(path);

    return byte_code;
}
