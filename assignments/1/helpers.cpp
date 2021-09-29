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

// // FOR TESTING - uncomment this and run "g++ helpers.cpp" then "./a.out"
// int main() {
//     vector<string> t;
//     string s1;
//     string s2;
//     s1 = "string1";
//     s2 = "string2";
//     t.push_back(s1);
//     t.push_back(s2);
//     cout << join(t) << endl;

//     return 1;
// }