#include <string>

using namespace std;

/**
 * Thread-safe write to log. Writes event to log file and updates stats in critical section.
 */
void safeLog(string logfile, int id, string event, int q=-1, int n=-1);

/**
 * Thread-safe add to queue. Adds to queue and notifies a waiting thread in critical section.
 */
void safeAdd(pair<char, int> command);

/**
 * Thread-safe get from queue. Retrieves work from queue and notifies producer to add more in critical section.
 * Also handles case where the done flag is set while waiting.
 */
pair<char, int> safeGet(int id);

/**
 * Continually ask for, take, and process work from queue. Exits when
 * queue empty and producer done.
 */
void consumer(int id);

/**
 * Main program to read cmd line args, initialize vars, spawn threads,
 * and run producer loop to read command input and add commands to queue
 */
int main(int argc, char **argv);