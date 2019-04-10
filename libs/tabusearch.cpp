#include <random>

#include "tabusearch.h"

/* ------- TESTED ------- */
int random_component(std::mt19937 &gen, int n)
{
    // This method selects a component solution (it can be used for leaving or entering)
    // The components have equal probability of being selected (uniformly distributed)
    std::uniform_int_distribution<int> distribution(0,n-1);
    // selects in the interval [0,n-1]
    int a = distribution(gen);
    return a;
}

/* ------- TESTED ------- */
int random_solution(std::mt19937 &gen, Solution &s, 
                    int size, int min, int max, 
                    int num_times_used[])
{
    std::cout << "begin random_solution\n";
    int fitness[size];
    for (int i = 0; i < size; ++i) fitness[i] = 1;
        // The probability of a beam being selected can deacrease if desired (num_times_used != 0)
        // If this diversification is not desired, all beams have uniform probability

    if (num_times_used != 0)
    {
        std::cout << "num_times_used != 0\n\n";
        int usage = std::accumulate(num_times_used, num_times_used+size, 0);
        for (int i = 0; i < size; ++i)
            fitness[i] = usage - num_times_used[i];
    }
    
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

    std::cout << "fitness:";
    for (int i = 0; i < size; ++i)
    {
        std::cout << " " << fitness[i];
    }
    
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

/* ------- TESTED ------- */
void init_tabu_list(int tabu_list[], int size, int tenure, int num_times_used[])
{
    for (int i = 0; i < size; i++)
    {
        for (int j = i+1; j < size; j++)
            tabu_list[i*size+j] = -tenure - 1;
        if(num_times_used != 0) num_times_used[i] = 0;

        // The diagonal will store the iteration at which the insertion/removal of the element i was done
        tabu_list[i*size+i] = -tenure;
    }
}

bool isTabu(int l, int e, int tabu_list[], int size, int tenure, int it)
{
    if(e < l) std::swap(e,l);

    if (tabu_list[(l-1)*size+(e-1)] + tenure >= it)
        return true;
    return false;
}

void make_tabu(int l, int e, int tabu_list[], int size, int it)
{
    if(e < l) std::swap(e,l);
    tabu_list[(l-1)*size+(e-1)] = it;
}