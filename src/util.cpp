#include "util.h"

#include <DirectXMath.h>
#include <fstream>
#include <string>

DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
    return DirectX::XMFLOAT3{a.x + b.x, a.y + b.y, a.z + b.z};
}

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b)
{
    return DirectX::XMFLOAT3{a.x - b.x, a.y - b.y, a.z - b.z};
}

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a)
{
    return DirectX::XMFLOAT3{a.x, a.y, a.z};
}

DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 v, float f)
{
    return DirectX::XMFLOAT3{v.x * f, v.y * f, v.z * f};
}

std::string load_file(const std::string &file_name)
{
    std::ifstream fs(file_name, std::ios::binary | std::ios::ate);

    if (!fs.good())
    {
        std::string s = "failed to open file: " + file_name;
        throw std::runtime_error(s.c_str());
    }

    size_t n = fs.tellg();
    fs.seekg(std::ios::beg);

    std::string byte_code(n, 0);

    fs.read(byte_code.data(), n);

    if (!fs)
    {
        throw std::runtime_error("failed to load byte code from: " + file_name);
    }

    return byte_code;
}
