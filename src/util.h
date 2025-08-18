#pragma once

#include <DirectXMath.h>
#include <intsafe.h>

#include <string>
#include <vector>

// DirectX Math
DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a);

DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 v, float f);

// Files
std::string load_file(const std::string &file_name);

// Memory
template <typename T> UINT load_vector(std::vector<T> &dest, T *src, size_t n)
{
    size_t i = dest.size(), new_size = i + n;

    dest.resize(new_size);

    memcpy(&dest[i], src, n * sizeof(T));

    return UINT(i);
}

// Debug
#ifndef NDEBUG

#include <sstream>
#include <stdexcept>

template <typename First> bool _build_string(std::stringstream &ss, First first)
{
    ss << first;
    return false;
}

template <typename First, typename... Rest> bool _build_string(std::stringstream &ss, First first, Rest... rest)
{
    ss << first;
    return _build_string(ss, rest...);
}

template <typename... Args> std::string build_string(Args... args)
{
    std::stringstream ss;

    while (_build_string(ss, args...))
        ;

    return ss.str();
}

#define MESSAGE(args)                                                                                                  \
    {                                                                                                                  \
        std::stringstream ss;                                                                                          \
        ss << args;                                                                                                    \
        throw std::runtime_error(ss.str());                                                                            \
    }

#else

#define MESSAGE(args)

#endif
