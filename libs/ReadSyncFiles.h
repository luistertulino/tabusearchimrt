#ifndef READSYNCFILES_H_
#define READSYNCFILES_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#define TS_SLEEP    0
#define MODEL_SLEEP 1
#define MODEL_STOP  2

#define RESULT_OK 0
#define RESULT_NOT_OK -1

using std::string;

const string sync_file    = "sync.txt";
const string angles_file  = "angles.txt";
const string weights_file = "weights.txt";
const string objs_file    = "objs.txt";

bool valid_state(int state);

int set_state(int state);

int state();

int set_angles(std::vector<int> &beam_set, int set_size);

int set_weights(double weights[], int num_weights);

double get_objs();

double get_objs(double objs[], int num_objs=3);

#endif