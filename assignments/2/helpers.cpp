#include "helpers.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <utility>
#include <string.h>
#include <stdio.h>
#include <sstream>
#include <unistd.h>
#include <chrono>
#include <typeinfo>
#include <map>

using namespace std;

/*
 * Read input from a commands input file and output a vector of char, int pairs
 * denoting a T or S command and integer time.
 */
vector<pair<char, int>> readCommands(string cmdsFile) {

// Initialize variables for file output and output vector
ifstream file(cmdsFile);
string line;
vector<pair<char, int>> commands;

    // Keep getting lines until we reach EOF
    while (getline(file, line)) {

        // Split into pair of first and second character, converting second character to an integer
        pair<char, int> cmd (line[0], line[1] - '0');
        commands.push_back(cmd);
    }
    return commands;

}

/*
 * Write info to a log file
 */
void writeLog(string logfile, float time, int id, string event, int q, int n) {

    // Open file
    FILE * outFile;
    outFile = fopen(logfile.c_str(), "a");

    // Write log line
    fprintf(outFile, "%8.3f ID= %d %4s %-12s %s\n", time, id, (q == -1) ? "" : ("Q= " + to_string(q)).c_str(), event.c_str(), (n == -1) ? "" : to_string(n).c_str());

    // Close file
    fclose(outFile);
}

/*
 * 
 */
void writeLogStats(string logfile, map<string, int> globalStats, map<int,int> threadStats) {
    // Open file
    FILE * outFile;
    outFile = fopen(logfile.c_str(), "a");

    // Write summary stats
    fprintf(outFile, "Summary:\n");
    // fprintf(outFile, "%.3f ID= %d %4s %-12s %s\n", time, id, (q == -1) ? "" : ("Q= " + to_string(q)).c_str(), event.c_str(), (n == -1) ? "" : to_string(n).c_str());
    for (auto it = globalStats.begin(); it != globalStats.cend(); ++it) {
        fprintf(outFile, "\t%-8s %6d\n", it->first.c_str(), it->second);
    }
    for (auto it = threadStats.begin(); it != threadStats.cend(); ++it) {
        fprintf(outFile, "\tThread %2d %5d\n", it->first, it->second);
    }

    // Close file
    fclose(outFile);
}

float getElapsed(chrono::time_point<chrono::high_resolution_clock> start) {
    auto end = chrono::high_resolution_clock::now();
    auto diff = end - start;
    auto seconds = chrono::duration <float> (diff).count();
    return (float) seconds;
}

// // FOR TESTING ONLY - Delete in submission
// int main() {

//     auto start = chrono::high_resolution_clock::now();
//     sleep(3);
//     printf("%.3f\n", getElapsed(start));

//     return 0;
// }