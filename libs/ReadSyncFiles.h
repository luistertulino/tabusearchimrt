#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define TS_SLEEP    0
#define MODEL_SLEEP 1
#define MODEL_STOP  2

using std::string;

const string sync_file    = "../sync.txt";
const string angles_file  = "../angles.txt";
const string weights_file = "../weights.txt";
const string objs_file    = "../objs.txt";

bool valid_state(int state)
{
    return (state == TS_SLEEP or state == MODEL_SLEEP or state == MODEL_SLEEP);
}

int set_state(int state)
{
    if(not valid_state(state)) return -1;

    std::ofstream outfile;
    outfile.open(sync_file, std::ios::out);
    if (outfile.is_open())
    {
        outfile << state;
        outfile.close();
        return 1;
    }

    std::cout << "Error in opening sync_file.\n";
    return -1;
}

int state()
{
    std::ifstream infile;
    infile.open(sync_file, std::ios::in);
    if (infile.is_open())
    {
        int s;
        infile >> s;
        infile.close();
        return s;
    }
    else
    {
        std::cout << "Error in opening sync_file.\n";
        return -1;
    }
}

int set_angles(std::vector<int> &beam_set)
{
    std::ofstream outfile;
    outfile.open(angles_file, std::ios::out);
    if (outfile.is_open())
    {
        for (auto b : beam_set) outfile << b << " ";
        outfile.close();
        return 1;
    }
    else
    {
        std::cout << "Error in opening angles_file.\n";
        return -1;
    }
}

int set_weights(std::vector<double> &weights)
{
    std::ofstream outfile;
    outfile.open(weights_file, std::ios::out);
    if (outfile.is_open())
    {
        for (auto w : weights) outfile << w << " ";
        outfile.close();
        return 1;
    }
    else
    {
        std::cout << "Error in opening weights_file.\n";
        return -1;
    }
}

double get_objs()
{
    std::ifstream infile;
    infile.open(objs_file, std::ios::in);
    if (infile.is_open())
    {
        double o = 0.0;
        infile >> o;
        infile.close();
        return o;
    }
    else
    {
        std::cout << "Error in opening objs_file.\n";
        return -1;
    }
}

double get_objs(double objs[], int num_objs=4)
{
    std::ifstream infile;
    infile.open(objs_file, std::ios::in);
    if (infile.is_open())
    {
        int i = 0.0;
        double o;
        while(infile >> o)
        {
            if (i == num_objs)
            {
                std::cout << "More objs in objs_file than the correct.\n";
                infile.close();
                return -1;
            }
            objs[i] = o;
            i++;
        }
        infile.close();

        if (i < num_objs)
        {
            std::cout << "Less objs in objs_file than the correct.\n";            
            return -1;
        }
        for (int j = 0; j < i; ++j)
        {
            std::cout << objs[j] << " ";
        }
        return 1;
    }
    else
    {
        std::cout << "Error in opening objs_file.\n";
        return -1;
    }
}
