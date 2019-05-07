#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <thread> // std::this_thread::sleep_for
#include <string>

#include "../libs/TSchainRad.h"

// best configuration found by irace tenure = 2, max_fails = 1

int main(int argc, char const *argv[])
{
    if (argc <= 1)
    {
        std::cout << "Not the right number of parameters!!!\n";
        std::cout << "It must be: ./main testcase_name execution_number tabu_tenure max_falhas [use_irace]\n";
        return -1;
    }

    std::string testcase(argv[1]);
    int execn = std::stoi(argv[2]);
    parameters p;
    p.num_beams = 15;
    p.min_beams = 1;
    p.max_beams = 15;
    p.max_time = 30;
    p.tabu_tenure = std::stoi(argv[3]);
    p.max_fails = std::stoi(argv[4]);
    p.outfile  = "results/"+testcase+"/objs." +std::to_string(execn)+".txt";
    p.beamfile = "results/"+testcase+"/beams."+std::to_string(execn)+".txt";

    modify_file_names(testcase, std::string(argv[2]));

    std::string command = "julia libs/IntensityModel.jl "+testcase+" "+std::string(argv[2]);
    FILE *f = popen(command.c_str(), "w");    

    TSchainRad ts(p);    
    int result = ts.init();
    if(result == RESULT_OK)
    {
        if (!!argv[5] and std::string(argv[5]) == "use_irace")
        {
            send_obj_to_irace(testcase, p.tabu_tenure, p.max_fails, execn, ts.final_obj);
        }
    } 
        
    int s = set_state(MODEL_STOP);
    pclose(f);

    return 0;
}