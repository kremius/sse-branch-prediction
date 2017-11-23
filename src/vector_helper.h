#pragma once

#define LIBDIVIDE_USE_SSE2 1

// TODO: maybe we can do even better, because we know
// dividers at compile time
#include <libdivide.h>

#include <x86intrin.h>

// https://stackoverflow.com/questions/6996764/fastest-way-to-do-horizontal-float-vector-sum-on-x86
inline int __attribute__((always_inline))
hsum_epi32_sse2(__m128i x)
{
#ifdef __AVX__
    __m128i hi64  = _mm_unpackhi_epi64(x, x);           // 3-operand non-destructive AVX lets us save a byte without needing a mov
#else
    __m128i hi64  = _mm_shuffle_epi32(x, _MM_SHUFFLE(1, 0, 3, 2));
#endif
    __m128i sum64 = _mm_add_epi32(hi64, x);
    __m128i hi32  = _mm_shufflelo_epi16(sum64, _MM_SHUFFLE(1, 0, 3, 2));    // Swap the low two elements
    __m128i sum32 = _mm_add_epi32(sum64, hi32);
    return _mm_cvtsi128_si32(sum32);       // SSE2 movd
    //return _mm_extract_epi32(hl, 0);     // SSE4, even though it compiles to movd instead of a literal pextrd r32,xmm,0
}

namespace
{

const libdivide::divider<int32_t> fast_1(1);
const libdivide::divider<int32_t> fast_2(2);
const libdivide::divider<int32_t> fast_3(3);
const libdivide::divider<int32_t> fast_4(4);
const libdivide::divider<int32_t> fast_5(5);

}
