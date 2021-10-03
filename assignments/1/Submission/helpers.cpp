/**
 * CMPUT 379 Assignment 1
 * Justin Boileau | 1535268
 * 
 * Helper functions for working with std::vectors, std::strings, and chars defined here.
 * 
 **/

#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <string.h>

using namespace std;

vector<string> split(char *s, char const *delimeter) {
    vector<string> tokens;
    char* token = strtok(s, delimeter);
    while (token != NULL) {
        tokens.push_back(token);
        token = strtok(NULL, delimeter);
    }
    return tokens;
}

string join(vector<string> strings) {
    string joined = "";
    
    // Append each string in strings with a space seperator
    for (auto s : strings) {
        joined.append(s);
        joined.append(" ");
    }
    return joined;
}

string checkStartSplit(vector<string> tokens, char c) {
    string s;
    for (auto token : tokens) {
        if (token[0] == c) {
            s = token.substr(1, string::npos);
        }
    }
    return s;
}

vector<string> removeElement(vector<string> v, string s) {

    // Get iterator to string s in v
    auto it = find(v.begin(), v.end(), s);

    // Erase it if it exists in v
    if (it != v.end()) {
        v.erase(it);
    }
    return v;

}

vector<char*> strVec2CharVec(vector<string> v) {
    
    // Try converting vector or strings to char **
    vector<char*> cV;
    cV.reserve(v.size());

    for (size_t i = 0; i < v.size(); ++i) {
        cV.push_back(const_cast<char*>(v[i].c_str()));
    }

    return cV;
}