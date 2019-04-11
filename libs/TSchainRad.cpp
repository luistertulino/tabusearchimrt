#include "TSchainRad.h"

#include <utility> // for std::pair
#include <set>
#include <list>
#include <random>
#include <iostream>
#include <algorithm>
#include <ctime> // for measure elapsed time
#include <numeric> // for accumulate

std::random_device rd;
std::mt19937 gen(rd());


// ------ TESTED ------
void print_structures(int tabu_list[], int num_times_used[], 
                      int num_beams, Solution &c, Solution &b)
{
    // print tabu list
    std::cout << "TABU LIST:\n";
    for (int i = 0; i < num_beams; ++i)
    {
        for (int j = 0; j < num_beams; j++) std::cout << tabu_list[i*num_beams+j] << " ";
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


int TSchainRad::init()
{
    int tabu_list[num_beams*num_beams];
    int num_times_used[num_beams];
    init_tabu_list(tabu_list, num_beams, tabu_tenure, num_times_used);

    Solution current(min_beams, max_beams, num_beams);
    Solution best(min_beams, max_beams, num_beams);

    /*----------------------------------- INITIALIZATION -----------------------------------*/
    int r = random_solution(gen, current, num_beams, min_beams, max_beams);
    if(r != RESULT_OK)
    {
        std::cout << "Erro na geração de uma solucao aleatoria. Código: " << r << std::endl;
        return r;
    }

    time_t begin;
    time(&begin);

    current.obj = solve_model(current);
    if (current.obj == RESULT_NOT_OK)
        return RESULT_NOT_OK;
    
    best = current;
    for (int i = 0; i < current.num_used; i++)
        num_times_used[current.beam_set[i]-1] += 1;

    /*------------------------------- END OF INITIALIZATION -------------------------------*/

    // Structures for register which solution components were already selected
    bool is_selected[num_beams];
    for (int i = 0; i < num_beams; ++i) is_selected[i] = false;

    print_structures(tabu_list, num_times_used, num_beams, current, best);
    
    return RESULT_OK;

    /* --------------- TESTED INTERACTION BETWEEN PROGRAMS UNTIL THIS POINT --------------- */

    /* 
      Structure to maintain the chain of movements.
      We also use a pointer to indicate which was the last movement that implied the biggest global improvement
    */
    
    std::list< std::pair<int,int> > chain;
    auto last_best = chain.end();
    
    int it = 0;
    //int no = 0;
    bool improvement = false;

    time_t now;
    time(&now);

    while(difftime(now,begin) < max_time)
    {
        int i = 0, j = 0;
        int min = std::min(current.num_used, current.num_not_used);

        int last_best_obj = current.obj;

        /* 
            Ejection chain
            The number of movements is limited by the minimum between number of beams in and out of solution.

            Important detail: during a ejection chain, a component solution that enters must not leave.
        */
        std::uniform_int_distribution<int> rand_l(0,current.num_used-1);
        std::uniform_int_distribution<int> rand_e(0,current.num_not_used-1);
        
        while(i < min and j < max_fails)
        {
            // Select a beam to leave solution
            int l = rand_l(gen);
            while( is_selected[ current.beam_set[l] ] ) l = rand_l(gen);
            is_selected[ current.beam_set[l] ] = true;

            // Select a beam to enter the in solution
            int e = rand_e(gen);
            while( is_selected[ current.other_beams[e] ] ) e = rand_e(gen);
            is_selected[ current.other_beams[e] ] = true;

            current.swap_beams(l,e);
            double new_obj = solve_model(current); // Modify later to include weights

            if (new_obj == RESULT_NOT_OK) return RESULT_NOT_OK;

            // This movement is inserted into the chain
            bool is_tabu = isTabu(current.beam_set[l], current.other_beams[e], 
                                  tabu_list, num_beams, tabu_tenure, it);

            // Check tabu condition
            if (new_obj < best.obj)
            {
                //When the new solution improves the best one, it does not matter if the movement is tabu
                current.obj = new_obj;
                best.obj = new_obj;
                improvement = true;

                auto move = std::make_pair(l,e);
                chain.insert(chain.begin(), move);
                last_best = chain.begin();
                last_best_obj = new_obj;
            }
            else if (not is_tabu)
            {
                // When the new solution improves only the current solution, 
                // the movement is considered only if it's not tabu
                current.obj = new_obj;
                auto move = std::make_pair(l,e);
                chain.insert(chain.begin(), move);
                if (new_obj < last_best_obj)
                {
                    last_best = chain.begin();
                    last_best_obj = new_obj;
                }
                else j++;
            }
            else current.swap_beams(l,e);
              // If the movement is tabu and does not improve anything, it is discarded              

            i++;
        }
        
        if (last_best == chain.end())
        {
            // No movement was able to improve the solution
            int r = random_solution(gen, current, num_beams, min_beams, max_beams, num_times_used);
            if(r != RESULT_OK) return r;
            current.obj = solve_model(current);

            if (current.obj == RESULT_NOT_OK) return RESULT_NOT_OK;

            if(current.obj < best.obj) best = current;
        }
        else
        {
            // There is at least one movement that improved the solution
            auto itc = chain.begin();
            // Undo all the movements that did not improve the solution
            while(itc != last_best)
            {
                int l = itc->first; 
                int e = itc->second;
                current.swap_beams(l,e);
                itc++;
            }
            current.obj = last_best_obj;

            for (itc = last_best; itc != chain.end(); itc++)
            {
                int l = current.beam_set[itc->first];
                int e = current.beam_set[itc->second];
                make_tabu(l, e, tabu_list, num_beams, it);
            }

            if (improvement) best = current;
            // If the best solution was improved at some point

        }
        for (int i = 0; i < current.num_used; i++)
            num_times_used[current.beam_set[i]-1] += 1;
        
        chain.clear();
        last_best = chain.end();
        for (int i = 0; i < num_beams; ++i) is_selected[i] = false;
        //leaving_sel.clear();
        //entering_sel.clear();

        it++; //no++;
        time(&now);
    }

    double g = solve_model(best);
    if (g == RESULT_NOT_OK) return RESULT_NOT_OK;
    best.obj = g;

    if (write_solution(best) != RESULT_OK) return RESULT_NOT_OK;

    // It seems to work until here  
    return RESULT_OK;    
}

int TSchainRad::write_solution(Solution &s)
{
    std::ofstream outfile;
    outfile.open(results_file, std::ios::app);

    if(outfile.is_open())
    {
        //outfile << tabu_tenure << " " << max_falhas << "\n";
        for (int i = 0; i < s.num_used; ++i) outfile << s.beam_set[i] << " ";

        outfile << "\n" << s.obj << " ";
        outfile << s.functions[ORGANS_OBJ] << " ";
        outfile << s.functions[TUMOR_POS_OBJ] << " ";
        outfile << s.functions[TUMOR_NEG_OBJ] << "\n";
    }
    else
    {
        std::cout << "Error in opening results_file.\n";
        return RESULT_NOT_OK;
    }

    return RESULT_OK;
}