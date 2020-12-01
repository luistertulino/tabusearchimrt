# tabusearchimrt
Tabu search code for Geometry and Intensity problems in IMRT. Code for the algorithm of the following paper:

L. Tertulino, S. M. D. M. Maia, E. F. G. Goldbarg and M. C. Goldbarg, "TSchainRad: a new Tabu Search-Based Matheuristic for IMRT Optimization," _2019 8th Brazilian Conference on Intelligent Systems (BRACIS)_, Salvador, Brazil, 2019, pp. 305-310, doi: 10.1109/BRACIS.2019.00061.

We use the Tabu Search metaheuristic with an ejection chain procedure to obtain beam sets (the geometry problem). Implemented in C++.

Every beam set (a solution for geometry problem) induces an intensity model to be optimized. Implemented in Julia.

The different-language codes interact by file synchronization.
