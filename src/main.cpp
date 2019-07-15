#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <thread> // std::this_thread::sleep_for
#include <string>

#include "../libs/TSchainRad.h"

// best configuration found by irace tenure = 2, max_fails = 1

void read_objs_beams(const string &filename, int &beams);

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
    //p.num_beams = 15;
    read_objs_beams("testcases/"+testcase+".txt", p.num_beams);
    p.min_beams = 1;
    p.max_beams = p.num_beams;
    p.max_time = 3600;
    p.tabu_tenure = std::stoi(argv[3]);
    p.max_fails = std::stoi(argv[4]);
    p.objsfile = "results/"+testcase+"/objs." +std::to_string(execn)+".txt";
    p.beamfile = "results/"+testcase+"/beams."+std::to_string(execn)+".txt";
    p.avalfile = "results/"+testcase+"/avals."+std::to_string(execn)+".txt";

    modify_file_names(testcase, std::string(argv[2]));

    std::string command = "julia libs/IntensityModel.jl "+testcase+" "+std::string(argv[2]);
    FILE *f = popen(command.c_str(), "w");

    TSchainRad ts(p);    
    int result = ts.init();
    if(result == RESULT_OK)
    {
        std::cout << "RESULT OK\n";
        if (!!argv[5] and std::string(argv[5]) == "use_irace")
            send_obj_to_irace(testcase, p.tabu_tenure, p.max_fails, execn, ts.final_obj);
    } 
        
    int s = set_state(MODEL_STOP);
    pclose(f);

    return 0;
}

void read_objs_beams(const std::string &filename, int &beams)
{
    std::ifstream infile;
    infile.open(filename, std::ios::in);
    if (infile.is_open())
    {
        infile >> beams;
        infile.close();
    }
}