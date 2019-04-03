#ifndef SOLUTION_H_
#define SOLUTION_H_

#define NUM_OBJS 3
#define ORGANS_OBJ 0
#define TUMOR_POS_OBJ 1
#define TUMOR_NEG_OBJ 2
// Indexes where the values of each obj function is stored

#include "ReadSyncFiles.h"

class Solution 
{
  public:
    std::vector<int> beam_set;
    int num_used;

    std::vector<int> other_beams;
    int num_not_used;

    double obj;

    // DO NOT SET THIS UNTIL THE END OF THE OPTIMIZATION PROCESS!!!
    double functions[NUM_OBJS] = {0};
    std::vector< std::vector<double> > beamlet_doses;

    Solution(){ }

    Solution(int min, int max, int numB)
    {
        beam_set.resize(max, 0);
        other_beams.resize(numB-min, 0);
        obj = 0.0;
    }

    Solution & operator=(Solution &other)
    {
        if(&other == this) return *this;

        num_used = other.num_used;
        num_not_used = other.num_not_used;

        for (int i = 0; i < num_used; ++i)
            beam_set[i] = other.beam_set[i];

        for (int i = 0; i < num_used; ++i)
            other_beams[i] = other.other_beams[i];

        obj = other.obj;
        for (int i = 0; i < NUM_OBJS; ++i)
            functions[i] = other.functions[i];

        return *this;
    }

    void swap_beams(int l, int e)
    {
        int aux;
        aux = beam_set[l];
        beam_set[l] = other_beams[e];
        other_beams[e] = aux;
    }

    void print()
    {
        std::cout << "\nCurrent solution: ";
        for (int i = 0; i < num_used; ++i) std::cout << beam_set[i] << " ";
        std::cout << "\nRemaining beams: ";
        for (int i = 0; i < num_not_used; ++i) std::cout << other_beams[i] << " ";
        std::cout << "\n";
    }
};

#endif