#include <random>
#include <iostream>
#include <fstream>
#include <iomanip> // for std::setprecision
#include <string>

#include "tabusearch.h"

/* ------- TESTED ------- */
/*
int random_component(std::mt19937 &gen, int n)
{
    // This method selects a component solution (it can be used for leaving or entering)
    // The components have equal probability of being selected (uniformly distributed)
    std::uniform_int_distribution<int> distribution(0,n-1);
    // selects in the interval [0,n-1]
    int a = distribution(gen);
    return a;
}
*/

/* ---------------------------------------------------- */
/* -------------- TABU SEARCH FUNCTIONS --------------- */
/* ---------------------------------------------------- */

int random_solution(std::mt19937 &gen, Solution &s, 
                    int size, int min, int max, 
                    int num_times_used[])
{
    std::cout << "begin random_solution\n";
    int fitness[size];
    
    // The probability of a beam being selected can deacrease if desired (num_times_used != 0)
    // If this diversification is not desired, all beams have uniform probability
    if (num_times_used != 0)
    {
        std::cout << "num_times_used != 0\n\n";
        int usage = std::accumulate(num_times_used, num_times_used+size, 0);
        for (int i = 0; i < size; ++i)
            fitness[i] = usage - num_times_used[i];
    }
    else for (int i = 0; i < size; ++i) fitness[i] = 1;
    
    bool is_selected[size];
    for (int i = 0; i < size; ++i) is_selected[i] = false;

    // Select the number of beams
    int nb = min;
    if (min != max)
    {
        std::uniform_int_distribution<int> n_b(min,max);
        nb = n_b(gen);
    }
    std::cout << "num beams = " << nb << "\n";
    
    // Select the beam set
    std::discrete_distribution<int> r_beam(fitness, fitness+size);
    for (int i = 0; i < nb; )
    {
        // Each iteration selects a beam.
        // If the beam was selected before, the current selection is discarded
        int r = r_beam(gen);
        std::cout << "selected beam: " << r << "\n";
        if(is_selected[r]) continue;

        is_selected[r] = true;
        i++;
    }

    s.num_used = nb;
    s.num_not_used = size - nb;
    int j = 0, k = 0;
    for (int i = 0; i < size; ++i)
    {
        if (is_selected[i])
        {
            s.beam_set[j] = i+1;
            j++;
        }
        else
        {
            s.other_beams[k] = i+1;
            k++;
        }
    }
    std::cout << "end random_solution\n";
    return RESULT_OK;
}

void init_tabu_list(int tabu_list[], int size, int tenure, int num_times_used[])
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
            tabu_list[i*size+j] = 0;
        if(num_times_used != 0) num_times_used[i] = 0;
    }
}

bool isTabu(int l, int e, int tabu_list[], int size, int tenure, int it)
{
    if (tabu_list[(l-1)*size+(e-1)] > it)
        return true;
    return false;
}

void make_tabu(int l, int e, int tabu_list[], int size, int tenure, int it)
{
    tabu_list[(l-1)*size+(e-1)] = it + tenure;
    tabu_list[(e-1)*size+(l-1)] = it + tenure;
}

/* ---------------------------------------------------- */
/* --------------- AUXILIARY FUNCTIONS ---------------- */
/* ---------------------------------------------------- */

int write_solution(Solution &s, std::string &outfile, std::string &beamfile)
{
    std::ofstream r_outfile, b_outfile;
    r_outfile.open(outfile, std::ios::app);
    b_outfile.open(beamfile, std::ios::app);

    if(r_outfile.is_open() and b_outfile.is_open())
    {
        //outfile << tabu_tenure << " " << max_falhas << "\n";
        b_outfile << s.beam_set[0];
        for (int i = 1; i < s.num_used; ++i) b_outfile << " " << s.beam_set[i];
        b_outfile << "\n";

        r_outfile << std::fixed << std::setprecision(15); // 15 decimal places
        r_outfile << s.obj << " ";
        r_outfile << s.functions[ORGANS_OBJ] << " ";
        r_outfile << s.functions[TUMOR_POS_OBJ] << " ";
        r_outfile << s.functions[TUMOR_NEG_OBJ] << "\n";
    }
    else
    {
        std::cout << "Error in opening result files.\n";
        return RESULT_NOT_OK;
    }

    return RESULT_OK;
}