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

extern string sync_file;
extern string angles_file;
extern string weights_file;
extern string objs_file;

bool valid_state(int state);

int set_state(int state);

int state();

int set_angles(std::vector<int> &beam_set, int set_size);

int set_weights(double weights[], int num_weights);

double get_objs();

double get_objs(double objs[], int num_objs=3);

void modify_file_names(const string &testcase, const string &execn);

#endif