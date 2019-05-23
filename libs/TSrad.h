#ifndef TSRAD_H_
#define TSRAD_H_

#include <iostream>
#include <fstream>
#include <list>
#include <utility> // for std::pair

#include "tabusearch.h"
#include "model.h"

struct parameters
{
    int num_beams, min_beams, max_beams;
    double maxtime;
    int tabutenure;
    int num_neighbors;
    std::string objsfile;
    std::string beamfile;
    std::string avalfile;
};

class TSrad
{
  public:
    int num_beams, min_beams, max_beams;
    double maxtime;
    int tabutenure;
    int num_neighbors;
    std::string objsfile;
    std::string beamfile;
    std::string avalfile;

    double final_obj;
    int num_avals;   

    TSrad();
    TSrad(parameters &p)
    {
        maxtime = p.maxtime;
        tabutenure = p.tabutenure;
        num_neighbors = p.num_neighbors;

        num_beams = p.num_beams;
        min_beams = p.min_beams;
        max_beams = p.max_beams;

        objsfile = p.objsfile;
        beamfile = p.beamfile;
        avalfile = p.avalfile;

        final_obj = 0.0;
        num_avals = 0;
    }

    int init(); // Tabu search functions
};

#endif