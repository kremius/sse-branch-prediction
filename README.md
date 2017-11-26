# SSE branch prediction
Some tests on SSE and branch prediction

It is a test project which was made in order to test some approaches to speed up the certain atmos calculation task (https://github.com/griefly/griefly/issues/387).
Overall results:
1. Branchless versions don't give any advantage here because amount of needed operations doubles in average.
2. Branching by code versions don't give any advantages compared to just fast division versions. Seems like modern CPUs can handle `if`s very well, at least in given conditions.
3. Integer division, however, is _very_ slow. A direct hint that divisors from 1 to 5 are needed gives a ~25% speed up.
4. SSE instructions help as well. Because there are no integer division in the SSE instructions set a switch by divisors is used everywhere in SSE. Overall speed up compared to simple serial version is ~40%.
5. But in the end the bottleneck might be not the calculations which performs `ProcessFiveCells` functions, but the cost of the memory load to cache outweigh calculations in our case. The best version (SSE) gives only ~30% performance gain compared to the simple version when there are no any internal loop (i.e. memory should be loaded before each processing). So another approach to the issue is to work onto memory more. But it will be different research.
