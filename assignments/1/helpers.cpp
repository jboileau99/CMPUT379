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

string checkStartSplit(vector<string> tokens, const char *c) {
    for (auto s : tokens) {
        cout << s << endl;// << " =?= " << endl << c << endl;
        // if (strcmp(s[0], c)) {
        //     return s.substr(1, string::npos);
        // }
    }
    //return NULL;
}