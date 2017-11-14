#include "atmos.h"

inline void __attribute__((always_inline))
ProcessFiveCells(Cell* near_cells[5])
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
    switch (near_size)
    {
    case 1:
        for (int i = 0; i < GASES_NUM; ++i)
        {
            gases_average[i] = gases_sums[i] / 1;
            gases_remains[i] = gases_sums[i] % 1;
        }
        break;
    case 2:
        for (int i = 0; i < GASES_NUM; ++i)
        {
            gases_average[i] = gases_sums[i] / 2;
            gases_remains[i] = gases_sums[i] % 2;
        }
        break;
    case 3:
        for (int i = 0; i < GASES_NUM; ++i)
        {
            gases_average[i] = gases_sums[i] / 3;
            gases_remains[i] = gases_sums[i] % 3;
        }
        break;
    case 4:
        for (int i = 0; i < GASES_NUM; ++i)
        {
            gases_average[i] = gases_sums[i] / 4;
            gases_remains[i] = gases_sums[i] % 4;
        }
        break;
    case 5:
        for (int i = 0; i < GASES_NUM; ++i)
        {
            gases_average[i] = gases_sums[i] / 5;
            gases_remains[i] = gases_sums[i] % 5;
        }
        break;
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

