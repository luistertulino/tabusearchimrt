#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <thread> // std::this_thread::sleep_for
#include <string>

#include "../libs/TSrad.h"

// tenure = 2

int main(int argc, char const *argv[])
{
    if (argc <= 1)
    {
        std::cout << "Not the right number of parameters!!!\n";
        std::cout << "It must be: ./main testcase_name execution_number tabu_tenure [use_irace]\n";
        return -1;
    }

    std::string testcase(argv[1]);
    int execn = std::stoi(argv[2]);
    parameters p;
    p.num_beams = 15;
    p.min_beams = 4;
    p.max_beams = 4;
    p.maxtime = 3;
    p.num_neighbors = 10;
    p.tabutenure = std::stoi(argv[3]);
    p.objsfile = "resultsobal/"+testcase+"/objs." +std::to_string(execn)+".txt";
    p.beamfile = "resultsobal/"+testcase+"/beams."+std::to_string(execn)+".txt";
    p.avalfile = "resultsobal/"+testcase+"/avals."+std::to_string(execn)+".txt";

    modify_file_names(testcase, std::string(argv[2]));

    std::string command = "julia libs/IntensityModel.jl "+testcase+" "+std::string(argv[2]);
    FILE *f = popen(command.c_str(), "w");    

    TSrad ts(p);    
    int result = ts.init();
    /*if(result == RESULT_OK)
    {
        if (!!argv[5] and std::string(argv[5]) == "use_irace")
            send_obj_to_irace(testcase, p.tabutenure, p.maxfails, execn, ts.final_obj);
    }*/ 
        
    int s = set_state(MODEL_STOP);
    pclose(f);

    return 0;
}