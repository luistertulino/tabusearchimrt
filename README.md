# tabusearchimrt
Tabu search code for Geometry and Intensity problems in IMRT

We use the Tabu Search metaheuristic with an ejection chain procedure to obtain beam sets (the geometry problem). Implemented in C++.
Every beam set (a solution for geometry problem) induces an intensity model to be optimized. Implemented in Julia.
The different-language codes interact by file synchronization.
