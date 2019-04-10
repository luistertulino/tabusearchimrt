#include "Solution.h"

#include <iostream>

Solution & Solution::operator=(Solution &other)
{
    if(&other == this) return *this;

    num_used = other.num_used;
    num_not_used = other.num_not_used;

    for (int i = 0; i < num_used; ++i)
        beam_set[i] = other.beam_set[i];

    for (int i = 0; i < num_not_used; ++i)
        other_beams[i] = other.other_beams[i];

    obj = other.obj;
    for (int i = 0; i < NUM_OBJS; ++i)
        functions[i] = other.functions[i];

    return *this;
}

void Solution::swap_beams(int l, int e)
{
    int aux;
    aux = beam_set[l];
    beam_set[l] = other_beams[e];
    other_beams[e] = aux;
}

void Solution::print()
{
    std::cout << "\nnum_used: " << num_used;
    std::cout << "\nBeam set: ";
    for (int i = 0; i < num_used; ++i) std::cout << beam_set[i] << " ";
    std::cout << "\n num_not_used: " << num_not_used;
    std::cout << "\nRemaining beams: ";
    for (int i = 0; i < num_not_used; ++i) std::cout << other_beams[i] << " ";
    std::cout << "\n";
}