#include "model.h"

#include <iostream>
#include <thread>

void ready()
{
    int s = state();
    while(s != MODEL_SLEEP)
    {
        std::cout << "state: " << s << ". sleeping\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(SLEEP));
        s = state();
    }
    
}

double solve_model(Solution &s, int report)
{
    // Weights used in Obal's Thesis
    double weights[NUM_OBJS] = {0.2, 0.0, 0.8};
    return solve_model(s, weights, report);
}

double solve_model(Solution &s, double weights[], int report)
{
    ready(); // wait until julia program is ready to solve

    int r = set_angles(s.beam_set, s.num_used);
    r = set_weights(weights, NUM_OBJS);

    if(r == RESULT_NOT_OK)
    {
        std::cout << "Error in setting angles and/or weights.\n";
        return RESULT_NOT_OK;
    }

    // The tabu search will wait until the intensity model on Julia program is solved
    r = set_state(TS_SLEEP);
    if (r == RESULT_NOT_OK)
    {
        std::cout << "Error in setting state to 0.\n";
        return RESULT_NOT_OK;
    }

    ready(); // Keep sleeping for 0.5s until the model solving is done

    double obj = get_objs(s.functions);
    std::cout << "obj = " << obj << "\n";
    if (obj == RESULT_NOT_OK)
    {
        std::cout << "Error in getting objs.\n";
        return RESULT_NOT_OK;
    }
    for (int i = 0; i < NUM_OBJS; ++i)
        std::cout << "f["<<i<<"] = " << s.functions[i] << "   " << (s.functions[i] - (int)s.functions[i]) << "\n";

    return obj;
}