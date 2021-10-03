/**
 * CMPUT 379 Assignment 1
 * Justin Boileau | 1535268
 * 
 * Main shell program for getting input, parsing, and dispatching commands.
 * 
 **/

#include <string>
#include <vector>

#ifndef SHELL_FUNCTIONS
#define SHELL_FUNCTIONS

using namespace std;

// Main shell loop
int main();

// Prints a shell379 text box
void initShell();

// Gets a line from the shell, returns vector of space delimited tokens
vector<string> getInput();

// Determines command parameters, forks, updates process table
void dispatchCmd(vector<string> tokens, string inFile, string outFile, bool, bool externalCmd, bool background);

#endif