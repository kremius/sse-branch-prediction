#include "atmos.h"

#include <array>

#include "vector_helper.h"

const int32_t YES = 0xFFFFFFFF;
const int32_t NO = 0x00000000;

constexpr std::array<int32_t, 16> InitMasks(int8_t direction)
{
    std::array<int32_t, 16> retval{};

    for (int i = 0; i < 16; ++i)
    {
        retval[i] = YES;
        if (i & direction)
        {
            retval[i] = NO;
	    }
    }

    return retval;
}

constexpr std::array<int32_t, 16> MASKS_NORTH = InitMasks(NORTH);
constexpr std::array<int32_t, 16> MASKS_SOUTH = InitMasks(SOUTH);
constexpr std::array<int32_t, 16> MASKS_WEST = InitMasks(WEST);
constexpr std::array<int32_t, 16> MASKS_EAST = InitMasks(EAST);

inline void __attribute__((always_inline))
ProcessFiveCells(int8_t attribute, Cell* near_cells[5])
{
    int32_t near_size = (4 - __builtin_popcount(attribute)) + 1;
    __m128i gases_sums = _mm_setzero_si128();

    // West, north, south, east - dirs
    int32_t mask_west = MASKS_WEST[attribute];
    int32_t mask_north = MASKS_NORTH[attribute];
    int32_t mask_south = MASKS_SOUTH[attribute];
    int32_t mask_east = MASKS_EAST[attribute];

    __m128i vector_mask_west = _mm_set1_epi32(MASKS_WEST[attribute]);
    __m128i vector_mask_north = _mm_set1_epi32(MASKS_NORTH[attribute]);
    __m128i vector_mask_south = _mm_set1_epi32(MASKS_SOUTH[attribute]);
    __m128i vector_mask_east = _mm_set1_epi32(MASKS_EAST[attribute]);

    // Start sum gases
    {
        __m128i nearby_gases = _mm_load_si128(
            reinterpret_cast<const __m128i*>(near_cells[0]->gases));
        nearby_gases = _mm_and_si128(nearby_gases, vector_mask_west);
        gases_sums = _mm_add_epi32(gases_sums, nearby_gases);
    }

    {
        __m128i nearby_gases = _mm_load_si128(
            reinterpret_cast<const __m128i*>(near_cells[1]->gases));
        nearby_gases = _mm_and_si128(nearby_gases, vector_mask_north);
        gases_sums = _mm_add_epi32(gases_sums, nearby_gases);
    }

    {
        __m128i nearby_gases = _mm_load_si128(
            reinterpret_cast<const __m128i*>(near_cells[2]->gases));
        nearby_gases = _mm_and_si128(nearby_gases, vector_mask_south);
        gases_sums = _mm_add_epi32(gases_sums, nearby_gases);
    }

    {
        __m128i nearby_gases = _mm_load_si128(
            reinterpret_cast<const __m128i*>(near_cells[3]->gases));
        nearby_gases = _mm_and_si128(nearby_gases, vector_mask_east);
        gases_sums = _mm_add_epi32(gases_sums, nearby_gases);
    }

    {
        __m128i nearby_gases = _mm_load_si128(
            reinterpret_cast<const __m128i*>(near_cells[4]->gases));
        gases_sums = _mm_add_epi32(gases_sums, nearby_gases);
    }

    // Finish sum gases

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

    {
        Cell& nearby = *near_cells[0];
        __m128i nearby_gases = _mm_load_si128(
            reinterpret_cast<const __m128i*>(nearby.gases));
        __m128i gases_diff = _mm_sub_epi32(gases_average, nearby_gases);
        int32_t sum_diff = hsum_epi32_sse2(gases_diff) & mask_west;
        nearby.flows[3] += sum_diff;
        center.flows[0] -= sum_diff;
        __m128i value = _mm_andnot_si128(vector_mask_west, nearby_gases);
        value = _mm_add_epi32(value, _mm_and_si128(gases_average, vector_mask_west));
        _mm_store_si128(reinterpret_cast<__m128i*>(nearby.gases), value);
    }

    {
        Cell& nearby = *near_cells[1];
        __m128i nearby_gases = _mm_load_si128(
            reinterpret_cast<const __m128i*>(nearby.gases));
        __m128i gases_diff = _mm_sub_epi32(gases_average, nearby_gases);
        int32_t sum_diff = hsum_epi32_sse2(gases_diff) & mask_north;
        nearby.flows[2] += sum_diff;
        center.flows[1] -= sum_diff;
        __m128i value = _mm_andnot_si128(vector_mask_north, nearby_gases);
        value = _mm_add_epi32(value, _mm_and_si128(gases_average, vector_mask_north));
        _mm_store_si128(reinterpret_cast<__m128i*>(nearby.gases), value);
    }

    {
        Cell& nearby = *near_cells[2];
        __m128i nearby_gases = _mm_load_si128(
            reinterpret_cast<const __m128i*>(nearby.gases));
        __m128i gases_diff = _mm_sub_epi32(gases_average, nearby_gases);
        int32_t sum_diff = hsum_epi32_sse2(gases_diff) & mask_south;
        nearby.flows[1] += sum_diff;
        center.flows[2] -= sum_diff;
        __m128i value = _mm_andnot_si128(vector_mask_south, nearby_gases);
        value = _mm_add_epi32(value, _mm_and_si128(gases_average, vector_mask_south));
        _mm_store_si128(reinterpret_cast<__m128i*>(nearby.gases), value);
    }

    {
        Cell& nearby = *near_cells[3];
        __m128i nearby_gases = _mm_load_si128(
            reinterpret_cast<const __m128i*>(nearby.gases));
        __m128i gases_diff = _mm_sub_epi32(gases_average, nearby_gases);
        int32_t sum_diff = hsum_epi32_sse2(gases_diff) & mask_east;
        nearby.flows[0] += sum_diff;
        center.flows[3] -= sum_diff;
        __m128i value = _mm_andnot_si128(vector_mask_east, nearby_gases);
        value = _mm_add_epi32(value, _mm_and_si128(gases_average, vector_mask_east));
        _mm_store_si128(reinterpret_cast<__m128i*>(nearby.gases), value);
    }

    gases_average = _mm_add_epi32(gases_average, gases_remains);
    _mm_store_si128(reinterpret_cast<__m128i*>(center.gases), gases_average);
}

void Process(CellsGroup* group)
{
    for (int i = CellsGroup::SIZE; i < (CellsGroup::SIZE * (CellsGroup::SIZE - 1)); ++i)
    {
        const int8_t attribute = static_cast<int8_t>(group->attributes[i]);
        Cell* near_cells[5];
        near_cells[0] = &group->cells[i - CellsGroup::SIZE];
        near_cells[1] = &group->cells[i - 1];
        near_cells[2] = &group->cells[i + 1];
        near_cells[3] = &group->cells[i + CellsGroup::SIZE];
        near_cells[4] = &group->cells[i];

        ProcessFiveCells(attribute, near_cells);
    }
}

