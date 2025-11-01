#include "debug.h"

#ifndef NDEBUG
#include <iostream>
#endif

void log_matrix(const DirectX::XMMATRIX &m)
{
#ifndef NDEBUG
    DirectX::XMFLOAT4X4 f;
    DirectX::XMStoreFloat4x4(&f, m);

    std::cout << f._11 << ", " << f._21 << ", " << f._31 << ", " << f._41 << "\n" //
              << f._12 << ", " << f._22 << ", " << f._32 << ", " << f._42 << "\n" //
              << f._13 << ", " << f._23 << ", " << f._33 << ", " << f._43 << "\n" //
              << f._14 << ", " << f._24 << ", " << f._34 << ", " << f._44 << "\n";
#endif
}
