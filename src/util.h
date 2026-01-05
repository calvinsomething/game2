#pragma once

#include <DirectXMath.h>
#include <intsafe.h>

#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <memory_allocator/Adapter.h>
#include <memory_allocator/BlockAllocator.h>

// DirectX Math
DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a);

DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 v, float f);

DirectX::XMFLOAT3 normalize(DirectX::XMFLOAT3 f3);

// Files
std::string load_file(const std::string &file_name);

// Memory
//
template <typename T> using Allocator = Adapter<T, BlockAllocator>;

// Vector
template <typename T> using StdVector = std::vector<T, Allocator<T>>;

// Map
template <typename K, typename V>
using StdUnorderedMap = std::unordered_map<K, V, std::hash<K>, std::equal_to<K>, Allocator<std::pair<K const, V>>>;

// String
using StdString = std::basic_string<Allocator<char>>;

// Set
template <typename T> using StdSet = std::set<T, Allocator<T>>;

template <typename T, typename A> UINT load_vector(std::vector<T, A> &dest, T *src, size_t n)
{
    size_t i = dest.size(), new_size = i + n;

    dest.resize(new_size);

    memcpy(&dest[i], src, n * sizeof(T));

    return UINT(i);
}

// Strings
std::wstring to_wc(const std::string &s);

std::string to_mb(const std::wstring &ws);

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

std::string directory_from_file_name(const std::string &file_name);
