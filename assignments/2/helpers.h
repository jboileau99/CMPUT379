#include <string>
#include <utility>
#include <vector>
#include <chrono>
#include <map>

using namespace std;

/**
 * Write producer or consumer event to log file
 */
void writeLog(string logfile, float time, int id, string event, int q=-1, int n=-1);

/**
 * Write stats to logfile
 */
void writeLogStats(string logfile, map<string, int> globalStats, map<int,int> threadStats, float endTime);

/**
 * Get time since start time
 */
float getElapsed(chrono::time_point<chrono::high_resolution_clock> start);