#include <string>
#include <utility>
#include <vector>
#include <chrono>
#include <map>

using namespace std;

vector<pair<char, int>> readCommands(string cmdsFile);
void writeLog(string logfile, float time, int id, string event, int q=-1, int n=-1);
void writeLogStats(string logfile, map<string, int> globalStats, map<int,int> threadStats);
float getElapsed(chrono::time_point<chrono::high_resolution_clock> start);