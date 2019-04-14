#include <random>
#include <string>

#include "Solution.h"
#include "ReadSyncFiles.h"

#ifndef TABUSEARCH_H_
#define TABUSEARCH_H_

//int random_component(std::mt19937 &gen, int n);

/* ---------------------------------------------------- */
/* -------------- TABU SEARCH FUNCTIONS --------------- */
/* ---------------------------------------------------- */

int random_solution(std::mt19937 &gen, Solution &s, 
                    int size, int min, int max, 
                    int num_times_used[]=0);

void init_tabu_list(int tabu_list[], int size, int tenure, int num_times_used[]=0);

bool isTabu(int l, int e, int tabu_list[], int size, int tenure, int it);

void make_tabu(int l, int e, int tabu_list[], int size, int tenure, int it);

/* ---------------------------------------------------- */
/* --------------- AUXILIARY FUNCTIONS ---------------- */
/* ---------------------------------------------------- */

int write_solution(Solution &s, std::string &outfile, std::string &beamfile); // Write results in a file

#endif