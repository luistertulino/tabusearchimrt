#ifndef TSCHAINRAD_H_
#define TSCHAINRAD_H_

//const std::string rfile = "results.txt";

#include <iostream>
#include <fstream>

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
    std::string results_file;
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
    std::string results_file;    

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

        results_file = p.results_file + "results.txt";

        //std::cout << "num_beams: " << num_beams << std::endl;
        //min_beams = model.min_beams;
        //std::cout << "min_beams: " << min_beams << std::endl;
        //max_beams = model.max_beams;
        //std::cout << "max_beams: " << max_beams << std::endl;
    }

    // Tabu search functions
    int init();
    //int random_solution(Solution &s, int num_times_used[]);
    //void init_tabu_list(int tabu_list[], int num_times_used[]);
    //bool isTabu(int l, int e, int tabu_list[], int it);
    //void make_tabu(int l, int e, int tabu_list[], int it);
    
    // Write result in a file
    int write_solution(Solution &s);
};

#endif