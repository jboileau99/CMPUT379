#include <string>
#include <vector>

#ifndef SHELL_FUNCTIONS
#define SHELL_FUNCTIONS

using namespace std;

int main();
void initShell();
vector<string> getInput();
void dispatchCmd(vector<string> tokens);

#endif