#include <iostream>
#include <vector>

#include <stdlib.h>

#include "atmos.h"

void FillGroups(std::vector<CellsGroup>* groups)
{
    srand(10891110);

    for (CellsGroup& group : *groups)
    {
        for (int i = 0; i < CellsGroup::SIZE * CellsGroup::SIZE; ++i)
        {
            for (int gas = 0; gas < GASES_NUM; ++gas)
            {
                group.cells[i].gases[gas] = rand() % 30000;
            }
            for (int dir = 0; dir < 4; ++dir)
            {
                group.cells[i].flows[dir] = 0;
            }
            group.attributes[i] = static_cast<Attribute>(rand() % 16);
        }
    }
}

void PrintFlows(const std::vector<CellsGroup>& groups)
{
    for (const CellsGroup& group : groups)
    {
        int64_t flows_sum[4] = {0, 0, 0, 0};
        for (int i = 0; i < CellsGroup::SIZE * CellsGroup::SIZE; ++i)
        {
            for (int dir = 0; dir < 4; ++dir)
            {
                flows_sum[dir] += group.cells[i].flows[dir];
            }
        }
        std::cout << "Flows: ";
        for (int dir = 0; dir < 4; ++dir)
        {
            std::cout << flows_sum[dir] << " ";
        }
        std::cout << std::endl;
    }
}

void Process(CellsGroup* group);

int main(int argc, char* argv[])
{
    std::cout << "Creating groups" << std::endl;

    const int GROUPS_AMOUNT = 128;
    std::vector<CellsGroup> groups(GROUPS_AMOUNT);

    std::cout << "Filling groups" << std::endl;

    FillGroups(&groups);

    for (int i = 0; i < 1024; ++i)
    {
        for (CellsGroup& group : groups)
        {
            Process(&group);
        }
    }
    PrintFlows(groups);

    return 0;
}
