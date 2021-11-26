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
#include <array>

using namespace std;

void writeLog(string logfile, float time, int id, string event, int q, int n) {

    // Open file
    FILE * outFile;
    outFile = fopen(logfile.c_str(), "a");

    // Write log line
    fprintf(outFile, "%8.3f ID= %d %4s %-12s %s\n", time, id, (q == -1) ? "" : ("Q= " + to_string(q)).c_str(), event.c_str(), (n == -1) ? "" : to_string(n).c_str());

    // Close file
    fclose(outFile);
}

void writeLogStats(string logfile, map<string, int> globalStats, map<int,int> threadStats, float endTime) {
    
    // Open file
    FILE * outFile;
    outFile = fopen(logfile.c_str(), "a");

    // Header
    fprintf(outFile, "Summary:\n");
    
    // Global stats
    array<string, 5> toPrint {"Work", "Ask", "Receive", "Complete", "Sleep"};
    for (string key : toPrint) {
        auto elem = globalStats.find(key);
        fprintf(outFile, "\t%-8s %7d\n", elem->first.c_str(), elem->second);
    }

    // Per-thread stats
    for (auto it = threadStats.begin(); it != threadStats.cend(); ++it) {
        fprintf(outFile, "\tThread %2d %6d\n", it->first, it->second);
    }
    fprintf(outFile, "Transactions per second: %-10.2f\n", globalStats["Work"]/endTime);

    // Close file
    fclose(outFile);
}

float getElapsed(chrono::time_point<chrono::high_resolution_clock> start) {

    // Get floating point difference between passed in time and now
    auto end = chrono::high_resolution_clock::now();
    auto diff = end - start;
    auto seconds = chrono::duration <float> (diff).count();
    return (float) seconds;
}