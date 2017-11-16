#include "atmos.h"

const int INDEXES_TO_DIRS[5] = {WEST, NORTH, SOUTH, EAST, NO_BORDERS};

template<int8_t attribute>
inline void __attribute__((always_inline))
ProcessFiveCells(Cell* near_cells[5])
{
    int32_t near_size = (4 - __builtin_popcount(attribute)) + 1;
    int32_t gases_sums[GASES_NUM];
    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_sums[i] = 0;
    }

    for (int dir = 0; dir < 5; ++dir)
    {
        if (!(attribute & INDEXES_TO_DIRS[dir]))
        {
            Cell* nearby = near_cells[dir];
            for (int i = 0; i < GASES_NUM; ++i)
            {
                gases_sums[i] += nearby->gases[i];
            }
        }
    }

    int32_t gases_average[GASES_NUM];
    int32_t gases_remains[GASES_NUM];
    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_average[i] = gases_sums[i] / near_size;
        gases_remains[i] = gases_sums[i] % near_size;
    }

    Cell& center = *near_cells[4];

    for (int dir = 0; dir < 4; ++dir)
    {
        if (!(attribute & INDEXES_TO_DIRS[dir]))
        {
            Cell& nearby = *near_cells[dir];
            for (int i = 0; i < GASES_NUM; ++i)
            {
                int diff = gases_average[i] - nearby.gases[i];
                nearby.flows[REVERT_DIRS_INDEXES[dir]] += diff;
                center.flows[dir] -= diff;
                nearby.gases[i] = gases_average[i];
            }
        }
    }

    for (int i = 0; i < GASES_NUM; ++i)
    {
        center.gases[i] = gases_average[i] + gases_remains[i];
    }
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

