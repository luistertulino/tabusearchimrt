#ifndef TSCHAINRAD_H_
#define TSCHAINRAD_H_

#include <iostream>
#include <fstream>
#include <list>
#include <vector>
#include <utility> // for std::pair

#include "tabusearch.h"
#include "model.h"

struct parameters
{
    int num_beams, min_beams, max_beams;
    int max_iterations, max_no_imp;
    double max_time;
    int tabu_tenure;
    //int changing_factor;
    int max_fails;
    std::string objsfile;
    std::string beamfile;
    std::string avalfile;
};

class TSchainRad
{
  public:
    int num_beams, min_beams, max_beams;
    int max_iterations, max_no_imp; // This will not be used
    double max_time;
    int tabu_tenure;
    int max_fails;
    std::string objsfile;
    std::string beamfile;
    std::string avalfile;

    double final_obj;
    int num_avals;   

    TSchainRad();
    TSchainRad(parameters &p)
    {
        max_time = p.max_time;
        tabu_tenure = p.tabu_tenure;
        max_fails = p.max_fails;

        num_beams = p.num_beams;
        min_beams = p.min_beams;
        max_beams = p.max_beams;

        objsfile = p.objsfile;
        beamfile = p.beamfile;
        avalfile = p.avalfile;

        final_obj = 0.0;
        num_avals = 0;
    }

    int init(); // Tabu search functions
    int evaluate_solution(std::vector<int> &beam_set);
};

#endif