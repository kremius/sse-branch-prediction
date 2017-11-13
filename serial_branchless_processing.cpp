#include "atmos.h"

#include <array>

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
    int32_t gases_sums[GASES_NUM] = {0, 0, 0, 0};

    // West, north, south, east - dirs
    int32_t mask_west = MASKS_WEST[attribute];
    int32_t mask_north = MASKS_NORTH[attribute];
    int32_t mask_south = MASKS_SOUTH[attribute];
    int32_t mask_east = MASKS_EAST[attribute];

    // Start sum gases
    {
        for (int i = 0; i < GASES_NUM; ++i)
        {
            gases_sums[i] += near_cells[0]->gases[i] & mask_west;
        }
    }

    {
        for (int i = 0; i < GASES_NUM; ++i)
        {
            gases_sums[i] += near_cells[1]->gases[i] & mask_north;
        }
    }

    {
        for (int i = 0; i < GASES_NUM; ++i)
        {
            gases_sums[i] += near_cells[2]->gases[i] & mask_south;
        }
    }

    {
        for (int i = 0; i < GASES_NUM; ++i)
        {
            gases_sums[i] += near_cells[3]->gases[i] & mask_east;
        }
    }

    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_sums[i] += near_cells[4]->gases[i];
    }

    // Finish sum gases

    int32_t gases_average[GASES_NUM];
    int32_t gases_remains[GASES_NUM];
    for (int i = 0; i < GASES_NUM; ++i)
    {
        gases_average[i] = gases_sums[i] / near_size;
        gases_remains[i] = gases_sums[i] % near_size;
    }

    Cell& center = *near_cells[4];

    {
        Cell& west = *near_cells[0];
        for (int i = 0; i < GASES_NUM; ++i)
        {
            int diff = (gases_average[i] - west.gases[i]) & mask_west;
            west.flows[3] += diff;
            center.flows[0] -= diff;
            west.gases[i] -= west.gases[i] & mask_west;
            west.gases[i] += gases_average[i] & mask_west;
        }
    }

    {
        Cell& north = *near_cells[1];
        for (int i = 0; i < GASES_NUM; ++i)
        {
            int diff = (gases_average[i] - north.gases[i]) & mask_north;
            north.flows[2] += diff;
            center.flows[1] -= diff;
            north.gases[i] -= north.gases[i] & mask_north;
            north.gases[i] += gases_average[i] & mask_north;
        }
    }

    {
        Cell& south = *near_cells[2];
        for (int i = 0; i < GASES_NUM; ++i)
        {
            int diff = (gases_average[i] - south.gases[i]) & mask_south;
            south.flows[1] += diff;
            center.flows[2] -= diff;
            south.gases[i] -= south.gases[i] & mask_south;
            south.gases[i] += gases_average[i] & mask_south;
        }
    }

    {
        Cell& east = *near_cells[3];
        for (int i = 0; i < GASES_NUM; ++i)
        {
            int diff = (gases_average[i] - east.gases[i]) & mask_east;
            east.flows[0] += diff;
            center.flows[3] -= diff;
            east.gases[i] -= east.gases[i] & mask_east;
            east.gases[i] += gases_average[i] & mask_east;
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

