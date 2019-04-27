#ifndef TSCHAINRAD_H_
#define TSCHAINRAD_H_

#include <iostream>
#include <fstream>
#include <list>
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
    std::string outfile;
    std::string beamfile;
};

class TSchainRad
{
  public:
    int num_beams, min_beams, max_beams;
    int max_iterations, max_no_imp; // This will not be used
    double max_time;
    int tabu_tenure;
    //int changing_factor;
    int max_fails;
    std::string outfile;
    std::string beamfile;

    double final_obj;   

    TSchainRad();
    TSchainRad(parameters &p)
    {
        //max_iterations = p.max_iterations;
        //max_no_imp = p.max_no_imp;
        max_time = p.max_time;
        tabu_tenure = p.tabu_tenure;
        //changing_factor = p.changing_factor;
        max_fails = p.max_fails;

        num_beams = p.num_beams;
        min_beams = p.min_beams;
        max_beams = p.max_beams;

        outfile = p.outfile;
        beamfile = p.beamfile;
    }

    int init(); // Tabu search functions
};

#endif