#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <vector>

#define NUM_OBJS 3
#define ORGANS_OBJ 0
#define TUMOR_POS_OBJ 1
#define TUMOR_NEG_OBJ 2
// Indexes where the values of each obj function is stored

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

    Solution & operator=(Solution &other);

    void swap_beams(int l, int e);
    void print();
};

#endif