#include "ReadSyncFiles.h"

bool valid_state(int state)
{
    return (state == TS_SLEEP or state == MODEL_SLEEP or state == MODEL_STOP);
}

int set_state(int state)
{
    if(not valid_state(state)) return RESULT_NOT_OK;

    std::ofstream outfile;
    outfile.open(sync_file, std::ios::out);
    if (outfile.is_open())
    {
        std::cout << "c++: new state: " << state << "\n";
        outfile << state;
        outfile.close();
        return RESULT_OK;
    }

    std::cout << "Error in opening sync_file.\n";
    return RESULT_NOT_OK;
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
        return RESULT_NOT_OK;
    }
}

int set_angles(std::vector<int> &beam_set, int set_size)
{
    std::cout << "c++: set_angles\n";
    std::ofstream outfile;
    outfile.open(angles_file, std::ios::out);
    if (outfile.is_open())
    {
        outfile << beam_set[0];
        for (int i = 1; i < set_size; ++i) outfile << " " << beam_set[i];
        outfile.close();
        return RESULT_OK;
    }
    else
    {
        std::cout << "Error in opening angles_file.\n";
        return RESULT_NOT_OK;
    }
}

int set_weights(double weights[], int num_weights)
{
    std::cout << "c++: set_weights\n";
    std::ofstream outfile;
    outfile.open(weights_file, std::ios::out);
    if (outfile.is_open())
    {
        outfile << weights[0];
        for (int i = 1; i < num_weights; ++i)
            outfile << " " << weights[i];
        outfile.close();
        return RESULT_OK;
    }
    else
    {
        std::cout << "Error in opening weights_file.\n";
        return RESULT_NOT_OK;
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
        return RESULT_NOT_OK;
    }
}

double get_objs(double objs[], int num_objs)
{
    std::ifstream infile;
    infile.open(objs_file, std::ios::in);
    if (infile.is_open())
    {
        int i = 0.0;
        double g, o;
        infile >> g;
        while(infile >> o)
        {
            if (i == num_objs)
            {
                std::cout << "More objs in objs_file than the correct.\n";
                infile.close();
                return RESULT_NOT_OK;
            }
            objs[i] = o;
            i++;
        }
        infile.close();

        if (i < num_objs)
        {
            std::cout << "Less objs in objs_file than the correct.\n";            
            return RESULT_NOT_OK;
        }
        return g;
    }
    else
    {
        std::cout << "Error in opening objs_file.\n";
        return RESULT_NOT_OK;
    }
}