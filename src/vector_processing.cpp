#include "atmos.h"

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

inline void __attribute__((always_inline))
ProcessFiveCells(Cell* near_cells[5])
{
    int32_t near_size = 0;
    __m128i gases_sums = _mm_setzero_si128();

    for (int dir = 0; dir < 5; ++dir)
    {
        if (Cell* nearby = near_cells[dir])
        {
            __m128i nearby_gases = _mm_load_si128(
                reinterpret_cast<const __m128i*>(nearby->gases));
            gases_sums = _mm_add_epi32(gases_sums, nearby_gases);
            ++near_size;
        }
    }

    // TODO: fast modulo with fast division without need for
    // extra multiplication

    __m128i gases_average;
    __m128i gases_remains;
    switch (near_size)
    {
    case 1:
        gases_average = fast_1.perform_divide_vector(gases_sums);
        break;
    case 2:
        gases_average = fast_2.perform_divide_vector(gases_sums);
        break;
    case 3:
        gases_average = fast_3.perform_divide_vector(gases_sums);
        break;
    case 4:
        gases_average = fast_4.perform_divide_vector(gases_sums);
        break;
    case 5:
        gases_average = fast_5.perform_divide_vector(gases_sums);
        break;
    default:
        __builtin_unreachable();
    }

    {
        __m128i temp = _mm_mullo_epi32(_mm_set1_epi32(near_size), gases_average);
        gases_remains = _mm_sub_epi32(gases_sums, temp);
    }

    Cell& center = *near_cells[4];

    for (int dir = 0; dir < 4; ++dir)
    {
        if (near_cells[dir])
        {
            Cell& nearby = *near_cells[dir];
            __m128i nearby_gases = _mm_load_si128(
                reinterpret_cast<const __m128i*>(nearby.gases));
            __m128i gases_diff = _mm_sub_epi32(gases_average, nearby_gases);
            int32_t sum_diff = hsum_epi32_sse2(gases_diff);
            nearby.flows[REVERT_DIRS_INDEXES[dir]] += sum_diff;
            center.flows[dir] -= sum_diff;
            _mm_store_si128(reinterpret_cast<__m128i*>(nearby.gases), gases_average);
        }
    }

    gases_average = _mm_add_epi32(gases_average, gases_remains);
    _mm_store_si128(reinterpret_cast<__m128i*>(center.gases), gases_average);
}

void Process(CellsGroup* group)
{
    for (int i = CellsGroup::SIZE; i < (CellsGroup::SIZE * (CellsGroup::SIZE - 1)); ++i)
    {
        const int8_t attribute = static_cast<int8_t>(group->attributes[i]);
        Cell* near_cells[5] = {nullptr, nullptr, nullptr, nullptr, &group->cells[i]};

        if (~attribute & WEST)
        {
            near_cells[0] = &group->cells[i - CellsGroup::SIZE];
        }
        if (~attribute & NORTH)
        {
            near_cells[1] = &group->cells[i - 1];
        }
        if (~attribute & SOUTH)
        {
            near_cells[2] = &group->cells[i + 1];
        }
        if (~attribute & EAST)
        {
            near_cells[3] = &group->cells[i + CellsGroup::SIZE];
        }

        ProcessFiveCells(near_cells);
    }
}

