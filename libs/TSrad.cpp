#include "TSrad.h"

#include <random>
#include <iostream>
#include <algorithm>
#include <ctime> // for measure elapsed time
#include <numeric> // for accumulate
#include <limits> // for double limit

#define REPORT 1

std::random_device rd;
std::mt19937 gen(rd());

// ------ TESTED ------
void print_structures(int tabulist[], int num_times_used[], 
                      int num_beams, Solution &c, Solution &b)
{
    // print tabu list
    std::cout << "TABU LIST:\n";
    for (int i = 0; i < num_beams; ++i)
    {
        for (int j = 0; j < num_beams; j++) std::cout << tabulist[i*num_beams+j] << " ";
        std::cout << std::endl;
    }

    std::cout << "\nnum_times_used:\n";
    for (int i = 0; i < num_beams; ++i) std::cout << num_times_used[i] << " ";

    std::cout << "\n\nCurrent solution:\n";
    c.print();
    std::cout << "\nBest Solution:\n";
    b.print();
    std::cout << "\n\n";
}

int TSrad::init()
{
    int tabulist[num_beams*num_beams];
    init_tabu_list(tabulist, num_beams, tabutenure);

    Solution current(min_beams, max_beams, num_beams);
    Solution best(min_beams, max_beams, num_beams);

    /*----------------------------------- INITIALIZATION -----------------------------------*/
    int r = random_solution(gen, current, num_beams, min_beams, max_beams);
    if(r != RESULT_OK)
    {
        std::cout << "Erro na geração de uma solucao aleatoria. Código: " << r << std::endl;
        return r;
    }

    time_t begin; time(&begin);

    current.obj = solve_model(current); num_avals++;
    if (current.obj == RESULT_NOT_OK) return RESULT_NOT_OK;    
    best = current;

    /*------------------------------- END OF INITIALIZATION -------------------------------*/

    // Structures for register which solution components were already selected
    int selected[2][num_neighbors];
    double obj[num_neighbors];
    for (int i = 0; i < num_neighbors; ++i)
    {
        selected[0][i] = selected[1][i] = 0;
        obj[i] = std::numeric_limits<double>::max();
    } 

    time_t now; time(&now);
    int it = 0;

    while(difftime(now,begin) < maxtime)
    {
        std::uniform_int_distribution<int> rand_l(0,current.num_used-1);
        std::uniform_int_distribution<int> rand_e(0,current.num_not_used-1);

        for(int i = 0; i < num_neighbors; i++)
        {
            int l, e;
            bool keep = true;
            while(keep)
            {
                l = rand_l(gen); e = rand_e(gen);
                keep = false;
                for (int j = 0; j < i; ++j) // Verify if that move was already made
                {
                    if (selected[0][j] == l and selected[1][j] == e)
                    {
                        keep = true;
                        break;
                    }
                }
            }
            current.swap_beams(l,e);
            double newobj = solve_model(current);
            num_avals++;

            if (newobj == RESULT_NOT_OK) return RESULT_NOT_OK;

            selected[0][i] = l; selected[1][i] = e; // mark that this move was made
            obj[i] = newobj; // save the objective function obtained
            current.swap_beams(l,e); // undo the move
        }

        bool moved = false;
        for (int i = 0; i < num_neighbors; ++i)
        {
            // Find out the min objective value and its corresponding move
            int pos = std::min_element(obj, obj+num_neighbors) - obj;
                           // min_element returns a pointer to the minimum element
            
            // Check tabu conditions
            bool tabu = isTabu(current.beam_set[ selected[0][pos] ], 
                               current.other_beams[ selected[1][pos] ], 
                               tabulist, num_beams, tabutenure, it);

            if (obj[pos] < best.obj or not tabu)
            {  // If the move satisfies aspiration criterium or is not tabu
                current.swap_beams( selected[0][pos], selected[1][pos] );
                make_tabu(selected[0][pos], selected[1][pos], tabulist, num_beams, tabutenure, it);
                current.obj = obj[pos];
                moved = true;
                break;
            }
            else obj[pos] = std::numeric_limits<double>::max(); // Else, look for the next min obj
        }

        if (moved and current.obj < best.obj) best = current;
        
        it++; time(&now);
    }

    double g = solve_model(best, REPORT);
    if (g == RESULT_NOT_OK) return RESULT_NOT_OK;
    best.obj = g;
    final_obj = best.obj;

    if (write_solution(best, num_avals, objsfile, beamfile, avalfile) != RESULT_OK) return RESULT_NOT_OK;

    // It seems to work until here  
    return RESULT_OK;    
}