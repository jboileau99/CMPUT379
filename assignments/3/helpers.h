#include <string>
#include <unordered_map>
#include <chrono>

using namespace std;

double getEpochTime();
void writeLogServer(string logfile, string event, int transaction, int n, string from);
void writeLogClient(string logfile, string event, int n);
void writeLogHeaderServer(string logfile, string port);
void writeLogHeaderClient(string logfile, string port, string server, string host);
void writeLogStatsServer(string logfile, unordered_map<string, int> client_stats, float elapsed_time, int transactions);
float getElapsed(chrono::time_point<chrono::high_resolution_clock> start);