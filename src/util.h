#include <DirectXMath.h>

#include <string>

// DirectX Math
DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a, DirectX::XMFLOAT3 b);

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 a);

DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 v, float f);

// Files
std::string load_file(const std::string &file_name);
