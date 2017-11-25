#include "atmos.h"

#include "vector_helper.h"

const int INDEXES_TO_DIRS[5] = {WEST, NORTH, SOUTH, EAST, NO_BORDERS};

template<int8_t attribute>
inline void __attribute__((always_inline))
ProcessFiveCells(Cell* near_cells[5])
{
    int32_t near_size = (4 - __builtin_popcount(attribute)) + 1;
    __m128i gases_sums = _mm_setzero_si128();

    for (int dir = 0; dir < 5; ++dir)
    {
        if (!(attribute & INDEXES_TO_DIRS[dir]))
        {
            Cell* nearby = near_cells[dir];
            __m128i nearby_gases = _mm_load_si128(
                reinterpret_cast<const __m128i*>(nearby->gases));
            gases_sums = _mm_add_epi32(gases_sums, nearby_gases);
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
        if (!(attribute & INDEXES_TO_DIRS[dir]))
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

        Cell* near_cells[5];
        near_cells[0] = &group->cells[i - CellsGroup::SIZE];
        near_cells[1] = &group->cells[i - 1];
        near_cells[2] = &group->cells[i + 1];
        near_cells[3] = &group->cells[i + CellsGroup::SIZE];
        near_cells[4] = &group->cells[i];

        switch (attribute)
        {
        case 0:
            ProcessFiveCells<0>(near_cells);
            break;
        case 1:
            ProcessFiveCells<1>(near_cells);
            break;
        case 2:
            ProcessFiveCells<2>(near_cells);
            break;
        case 3:
            ProcessFiveCells<3>(near_cells);
            break;
        case 4:
            ProcessFiveCells<4>(near_cells);
            break;
        case 5:
            ProcessFiveCells<5>(near_cells);
            break;
        case 6:
            ProcessFiveCells<6>(near_cells);
            break;
        case 7:
            ProcessFiveCells<7>(near_cells);
            break;
        case 8:
            ProcessFiveCells<8>(near_cells);
            break;
        case 9:
            ProcessFiveCells<9>(near_cells);
            break;
        case 10:
            ProcessFiveCells<10>(near_cells);
            break;
        case 11:
            ProcessFiveCells<11>(near_cells);
            break;
        case 12:
            ProcessFiveCells<12>(near_cells);
            break;
        case 13:
            ProcessFiveCells<13>(near_cells);
            break;
        case 14:
            ProcessFiveCells<14>(near_cells);
            break;
        case 15:
            ProcessFiveCells<15>(near_cells);
            break;
        default:
            __builtin_unreachable();
        }
    }
}

