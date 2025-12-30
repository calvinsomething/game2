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

DirectX::XMFLOAT3 normalize(DirectX::XMFLOAT3 f3)
{
    DirectX::XMVECTOR vec = DirectX::XMLoadFloat3(&f3);
    vec = DirectX::XMVector3Normalize(vec);
    DirectX::XMStoreFloat3(&f3, vec);
    return f3;
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

std::wstring to_wc(const std::string &s)
{
    std::wstring ws(s.size(), 0);

    for (size_t i = 0; i < s.size(); ++i)
    {
        mbtowc(ws.data() + i, &s[i], sizeof(char));
    }

    return ws;
}

std::string to_mb(const std::wstring &ws)
{
    std::string s(ws.size(), 0);

    for (size_t i = 0; i < s.size(); ++i)
    {
        wctomb_s(nullptr, &s[i], sizeof(wchar_t), ws[i]);
    }

    return s;
}

std::string directory_from_file_name(const std::string &file_name)
{
    size_t dir_end = file_name.find_last_of("/\\");

    if (dir_end == std::string::npos)
    {
        ERROR_MSG("invalid file path: " << file_name);
    }

    return file_name.substr(0, dir_end + 1);
}
