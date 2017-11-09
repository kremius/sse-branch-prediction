#pragma once

#include <memory>

const int GASES_NUM = 4;

struct alignas(16) Cell
{
    int32_t gases[GASES_NUM];
    // West, north, south, east
    int32_t flows[4];
};

const int REVERT_DIRS_INDEXES[4] = { 3, 2, 1, 0 };

using Attribute = int8_t;
// 0 borders
const Attribute NO_BORDERS = 0b0000;
// 4 borders
const Attribute FULL_BORDERS = 0b1111;
// 1 borders
const Attribute NORTH = 0b0001;
const Attribute SOUTH = 0b0010;
const Attribute WEST = 0b0100;
const Attribute EAST = 0b1000;
// 2 borders
const Attribute NORTH_SOUTH = NORTH | SOUTH;
const Attribute WEST_EAST = WEST | EAST;
const Attribute NORTH_WEST = NORTH | WEST;
const Attribute NORTH_EAST = NORTH | EAST;
const Attribute SOUTH_WEST = SOUTH | WEST;
const Attribute SOUTH_EAST = SOUTH | EAST;
// 3 borders
const Attribute EXCEPT_NORTH = ~NORTH & FULL_BORDERS;
const Attribute EXCEPT_SOUTH = ~SOUTH & FULL_BORDERS;
const Attribute EXCEPT_WEST = ~WEST & FULL_BORDERS;
const Attribute EXCEPT_EAST = ~EAST & FULL_BORDERS;

struct CellsGroup
{
    static const int SIZE = 32;

    CellsGroup()
        // Default malloc on linux return 16 bytes aligned memory
        : cells(new Cell[32 * 32]),
          attributes(new Attribute[32 * 32])
    {
        // Nothing
    }

    std::unique_ptr<Cell[]> cells;
    std::unique_ptr<Attribute[]> attributes;
};
