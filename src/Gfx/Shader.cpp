#include "Shader.h"

#include <d3dcompiler.h>

#include <fstream>
#include <stdexcept>

constexpr char SHADER_DIR[] = SHADER_SRC_DIR;

Shader::Shader(Gfx &gfx) : device(gfx.device), ctx(gfx.ctx)
{
}

std::vector<char> Shader::load(const char *file_name)
{
    std::string path(sizeof(SHADER_DIR) + 1 + strlen(file_name), '/');
    path.replace(0, sizeof(SHADER_DIR), SHADER_DIR);
    path.replace(sizeof(SHADER_DIR) + 1, path.size(), file_name);

    std::ifstream fs(path, std::ios::binary | std::ios::ate);

    if (!fs.good())
    {
        throw std::runtime_error("failed to open shader file");
    }

    size_t n = fs.tellg();
    fs.seekg(std::ios::beg);

    std::vector<char> byte_code(n);

    fs.read(byte_code.data(), n);

    if (!fs)
    {
        throw std::runtime_error("failed to load shader byte code");
    }

    return byte_code;
}
