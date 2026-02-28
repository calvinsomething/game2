#pragma once

#include "util.h"

constexpr size_t MB = size_t(1) << 20;

inline struct Init
{
    Init()
    {
        Allocator<int>::allocator.init(512 * MB, 10'000);
    }
} init;
