#include "atmos.h"

const int32_t YES = 0xFFFFFFFF;
const int32_t NO = 0x00000000;

const int32_t MASKS_NORTH[16]
    = {};

inline void __attribute__((always_inline))
ProcessFiveCells(int8_t attribute, Cell* near_cells[5])
{
    int32_t near_size = 0;
    int32_t gases_sums[GASES_NUM];
    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_sums[i] = 0;
    }

    for (int dir = 0; dir < 5; ++dir)
    {
        if (Cell* nearby = near_cells[dir])
        {
            for (int i = 0; i < GASES_NUM; ++i)
            {
                gases_sums[i] += nearby->gases[i];
            }
            ++near_size;
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
        if (near_cells[dir])
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

        ProcessFiveCells(attribute, near_cells);
    }
}

