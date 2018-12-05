#pragma once


#include "interface_header/base/platform.h"

namespace zq{


inline uint64 MAKE_PAIR64(uint32 l, uint32 h)
{
    return uint64(l | (uint64(h) << 32));
}

inline uint32 PAIR64_HIPART(uint64 x)
{
    return (uint32)((x >> 32) & UI64LIT(0x00000000FFFFFFFF));
}

inline uint32 PAIR64_LOPART(uint64 x)
{
    return (uint32)(x & UI64LIT(0x00000000FFFFFFFF));
}

inline uint16 MAKE_PAIR16(uint8 l, uint8 h)
{
    return uint16(l | (uint16(h) << 8));
}

inline uint32 MAKE_PAIR32(uint16 l, uint16 h)
{
    return uint32(l | (uint32(h) << 16));
}

inline uint16 PAIR32_HIPART(uint32 x)
{
    return (uint16)((x >> 16) & 0x0000FFFF);
}

inline uint16 PAIR32_LOPART(uint32 x)
{
    return (uint16)(x & 0x0000FFFF);
}

}
