#include "Shader.h"

#include <d3dcompiler.h>

#include <fstream>
#include <stdexcept>

constexpr char SHADER_DIR[] = SHADERS_BIN;

Shader::Shader(Gfx &gfx) : device(gfx.device.Get()), ctx(gfx.ctx.Get())
{
}

std::vector<char> Shader::load(const char *file_name)
{
    std::string path;
    path.reserve(sizeof(SHADER_DIR) + 1 + strlen(file_name));
    path += SHADER_DIR;
    path += '/';
    path += file_name;

    std::ifstream fs(path, std::ios::binary | std::ios::ate);

    if (!fs.good())
    {
        std::string s = "failed to open shader file: " + path;
        throw std::runtime_error(s.c_str());
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
