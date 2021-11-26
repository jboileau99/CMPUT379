#include "helpers.h"
#include "tands.c"

#include <iostream>
#include <vector>
#include <thread>
#include <queue>
#include <string>
#include <utility>
#include <unistd.h>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <atomic>
#include <map>

using namespace std;

// Global queue
queue<pair<char, int>> commands;

// Mutexes and condition variables for queue and log file, producer done flag
mutex queue_mutex, log_mutex, queue_full_mutex;
condition_variable queue_cv, log_cv, queue_full_cv;
atomic<bool> done(false);

// Stats objects
map<string, int> globalStats;  // Work, ask, receive, complete, sleep stats for the whole program
map<int, int> threadStats;  // Work completed by each thread

// Timing variable
auto start = chrono::high_resolution_clock::now();

// Log filename
string logfile;

void safeLog(string logfile, int id, string event, int q, int n) {

    // Get a lock and write to log file
    unique_lock<mutex> lck(log_mutex);
    // log_cv.wait(lck);
    writeLog(logfile, getElapsed(start), id, event, q, n);

    // Update the stats object
    globalStats[event]++;
    if (event == "Complete") {
        threadStats[id]++;
    }
}

void safeAdd(pair<char, int> command) {

    // Get a lock on the queue mutex to add a value
    lock_guard<mutex> lck(queue_mutex);
    commands.push(command);

    // Tell one of the waiting consumers that work is available
    queue_cv.notify_one();
}

pair<char, int> safeGet(int id) {

    // Get on the queue mutex to get new work
    unique_lock<mutex> lck(queue_mutex);

    // Initialize variable to hold the retrieved command
    pair<char, int> command;

    // Wait for work a notification of new work from the producer, IF the queue is currently empty and producer isn't done
    queue_cv.wait(lck, [](){return !commands.empty() || done;});
    
    // Return D0 if queue empty and producer is done
    if (commands.empty() && done) {
        command = pair<char, int> ('D', 0);
    } else {
        
        // Otherwise, get a command from the queue and remove it
        command = commands.front();
        commands.pop();
    }
    
    // Unlock queue mutex, i.e. leave critical section allowing another thread to get work
    lck.unlock();

    // Notify the producer to continue adding work incase it was waiting because queue was full
    queue_full_cv.notify_one();

    return command;
}

void consumer(int id) {
    
    while (!(done && commands.empty())) {  // Consumers keep going until producer is done and queue is empty

        // Ask for work and enter then enter thread safe get method - See readme for rational on logging outside of critical sections
        safeLog(logfile, id, "Ask", -1, -1);
        pair<char, int> command = safeGet(id);
        // printf("Thread %d - Got %c%d\n", id, command.first, command.second);

        if (command.first == 'T') {
            // Process and log for a T command
            safeLog(logfile, id, "Receive", commands.size(), command.second);
            Trans(command.second);
            safeLog(logfile, id, "Complete", -1, command.second);
        
        } else if (command.first == 'D') {
            // Producer signalled done and queue empty, so break
            break;
        } else {
            // Error if thread somehow got a different command
            throw logic_error("Consumer got work that wasn't T! This should never happen");
        }
    }

    // Notify all waiting threads that we are done processing
    if (commands.empty() && done) {
        queue_cv.notify_all();
    }
}

int main(int argc, char **argv) {

    // Save start time
    int nthreads;
    const char *fnum;
    int id = 0;

    // Determine nthreads and id
    if (argc == 2) {
        // Set ID to default value of 0
        nthreads = strtol(argv[1], nullptr, 0);
        fnum = NULL;
    } else if (argc == 3) {
        nthreads = strtol(argv[1], nullptr, 0);
        fnum = (char *) argv[2];
    } else if (argc < 2) {
        throw invalid_argument("Must specify at least an integer arguement indicating the number of threads.");
    } else {
        throw invalid_argument("Too many arguements.");
    }

    // Error check thread number
    if (nthreads <= 0) {
        throw invalid_argument("Cannot have negative or zero thread count.");
    }

    // Build logfile string
    if (fnum == NULL) {
        logfile = "prodcon.log";
    } else {
        logfile = "prodcon." + (string) fnum + ".log";
    }

    // Setup global and thread stats
    globalStats["Work"] = 0;
    globalStats["Ask"] = 0;
    globalStats["Receive"] = 0;
    globalStats["Complete"] = 0;
    globalStats["Sleep"] = 0;
    for (int i = 1; i < nthreads + 1; i++) { 
        // Did this in it's own loop instead of below to prevent race condition where threads start working before stats is intialized
        threadStats[i] = 0;
    }

    // Create thread variables (should do this as a vector eventually)
    vector<thread> threads;

    // Spawn consumer threads
    for (int i = 1; i < nthreads + 1; i++) { // Start at 1 so each consumer has ID > 0
        threads.push_back(thread(consumer, i));
    }

    // Get lock for queue full mutex so we can call wait with it, only producer users this
    unique_lock<mutex> lck(queue_full_mutex);

    /* Producer loop */
    for (string line; getline(cin, line);) {
        
        // Split into pair of first and second character, converting second character to an integer
        pair<char, int> cmd (line[0], stoi(line.substr(1, string::npos)));

        // When queue is full, producer waits for a notification from one of the consumers when work has been consumed
        queue_full_cv.wait(lck, [nthreads](){return (int) commands.size() < 2*nthreads;});

        // Producer adds work to queue
        if (cmd.first == 'T') {
            safeLog(logfile, id, "Work", commands.size(), cmd.second);
            safeAdd(cmd);
        } else {
            safeLog(logfile, id, "Sleep", -1, cmd.second);
            Sleep(cmd.second);
        }
    }

    // Producer done
    done = true;
    safeLog(logfile, id, "End", -1, -1);

    // Notify all consumers threads in case of specific interleaving that caused a thread to be stuck waiting while others have exited
    queue_cv.notify_all();

    // Wait for threads to complete once producer loop is done
    for (thread &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Write the stats summary
    writeLogStats(logfile, globalStats, threadStats, getElapsed(start));
    exit(0);
}