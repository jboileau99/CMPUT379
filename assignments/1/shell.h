#include <string>
#include <vector>

#ifndef SHELL_FUNCTIONS
#define SHELL_FUNCTIONS

using namespace std;

int main();
void initShell();
vector<string> getInput();
bool addProcess(int pid, char state, string command);
void dispatchCmd(vector<string> tokens, string inFile, string outFile, bool background);

#endif