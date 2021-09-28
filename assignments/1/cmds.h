#include <vector>
#include <string>

using namespace std;

#ifndef CMD_FUNCTIONS
#define CMD_FUNCTIONS

int _exit(vector<string> args);
int _jobs(vector<string> args);
int _kill(vector<string> args);
int _resume(vector<string> args);
int _sleep(vector<string> args);
int _suspend(vector<string> args);
int _wait(vector<string> args);
int _exec(vector<string> args);

#endif