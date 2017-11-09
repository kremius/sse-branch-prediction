#pragma once

#include <memory>

const int GASES_NUM = 4;

struct alignas(16) Cell
{
    int32_t gases[GASES_NUM];
    int32_t flows[4];
};

enum class Attribute : int8_t
{
    // 0 borders
    NO_BORDERS = 0,
    // 1 borders
    NORTH,
    SOUTH,
    WEST,
    EAST,
    // 2 borders
    NORTH_SOUTH,
    WEST_EAST,
    NORTH_WEST,
    NORTH_EAST,
    SOUTH_WEST,
    SOUTH_EAST,
    // 3 borders
    EXCEPT_NORTH,
    EXCEPT_SOUTH,
    EXCEPT_WEST,
    EXCEPT_EAST,
    // 4 borders
    FULL_BORDERS
};

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
    CellsGroup(CellsGroup&& other)
        : cells(std::move(other.cells)),
          attributes(std::move(other.attributes))
    {
        // Nothing
    }

    std::unique_ptr<Cell[]> cells;
    std::unique_ptr<Attribute[]> attributes;
};
