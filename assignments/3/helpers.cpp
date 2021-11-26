#include "helpers.h"

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <utility>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <unistd.h>
#include <chrono>
#include <typeinfo>
#include <unordered_map>
#include <array>

using namespace std;

double getEpochTime() {
    
    unsigned long milliseconds_since_epoch = 
    chrono::duration_cast<chrono::milliseconds>
        (chrono::high_resolution_clock::now().time_since_epoch()).count();

    return (double) milliseconds_since_epoch / 1000;
}

void writeLogServer(string logfile, string event, int transaction, int n, string from) {

    // Open file
    FILE * outFile;
    outFile = fopen(logfile.c_str(), "a");

    // Get epoch time
    double epoch = getEpochTime();

    // Write log depending on event
    if (event == "T") {
        fprintf(outFile, "%.2f: # %2d (T%3d) from %s\n", epoch, transaction, n, from.c_str());
    } else if (event == "D") {
        fprintf(outFile, "%.2f: # %2d (Done) from %s\n", epoch, transaction, from.c_str());
    } else {
        perror("Invalid log event");
        exit(EXIT_FAILURE);
    }

    // Close file
    fclose(outFile);
}

void writeLogClient(string logfile, string event, int n) {

    // Open file
    FILE * outFile;
    outFile = fopen(logfile.c_str(), "a");

    // Get epoch time
    double epoch = getEpochTime();

    // Write log depending on event
    if (event == "T") {
        fprintf(outFile, "%.2f: Send (T%3d)\n", epoch, n);
    } else if (event == "D") {
        fprintf(outFile, "%.2f: Recv (D%3d)\n", epoch, n);
    } else if (event == "S") {
        fprintf(outFile, "Sleep %d units\n", n);
    } else if (event == "E") {
        fprintf(outFile, "Sent %d transactions\n", n);
    } else {
        perror("Invalid log event");
        exit(EXIT_FAILURE);
    }

    // Close file
    fclose(outFile);
}

void writeLogHeaderServer(string logfile, string port) {

    // Open file
    FILE * outFile;
    outFile = fopen(logfile.c_str(), "a");

    // Write log lines
    fprintf(outFile, "Using port %s\n", port.c_str());

    // Close file
    fclose(outFile);

} 

void writeLogHeaderClient(string logfile, string port, string server, string host) {

    // Open file
    FILE * outFile;
    outFile = fopen(logfile.c_str(), "a");

    // Write log lines
    fprintf(outFile, "Using port %s\n", port.c_str());
    fprintf(outFile, "Using server address %s\n", server.c_str());
    fprintf(outFile, "Host %s\n", host.c_str());

    // Close file
    fclose(outFile);

}

void writeLogStatsServer(string logfile, unordered_map<string, int> client_stats, float elapsed_time, int transactions) {

    // Open file
    FILE * outFile;
    outFile = fopen(logfile.c_str(), "a");

    // Print stats for each client and single server
    fprintf(outFile, "\nSUMMARY\n");
    for (auto it = client_stats.begin(); it != client_stats.end(); ++it) {
        fprintf(outFile, "%4d transactions from %s\n", it->second, it->first.c_str());
    }
    fprintf(outFile, "%4.1f transactions/sec  (%d/%.2f)\n", transactions/elapsed_time, transactions, elapsed_time);

    // Close file
    fclose(outFile);
}

float getElapsed(chrono::time_point<chrono::high_resolution_clock> start) {

    // Get floating point difference between start and now
    auto end = chrono::high_resolution_clock::now();
    auto diff = end - start;
    auto seconds = chrono::duration <float> (diff).count();
    return (float) seconds;
}