#include <vector>
#include <map>

using namespace std;

#ifndef HELPER_FUNCTIONS
#define HELPER_FUNCTIONS

// Split a string s by some delimeter d and return a vector with each token
vector<string> split(char *s, char const *delimeter);
string join(vector<string> strings);

// Check for a character c at start of string, return rest of string if found
string checkStartSplit(vector<string> tokens, char c);

#endif