#ifndef MODEL_H_
#define MODEL_H_

#include "Solution.h"
#include "ReadSyncFiles.h"

#define SLEEP 500 // in miliseconds

void ready();

double solve_model(Solution &s, double weights[], int report=0);

double solve_model(Solution &s, int report=0);

#endif