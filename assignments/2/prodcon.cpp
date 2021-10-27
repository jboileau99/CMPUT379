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

// Mutexes and condition variables for queue and log file
mutex queue_mutex, log_mutex;
condition_variable queue_cv, log_cv;
atomic<bool> done(false);

// Stats objects
map<string, int> globalStats;  // Work, ask, receive, complete, sleep stats for the whole program
map<int, int> threadStats;  // Work completed by each thread

// Timing variable
auto start = chrono::high_resolution_clock::now();

// Log filename
string logfile;

void safeAdd(pair<char, int> command) {
    lock_guard<mutex> lck(queue_mutex);
    commands.push(command);
    queue_cv.notify_one();
}

// Safe get 1
pair<char, int> safeGet() {
    unique_lock<mutex> lck(queue_mutex);
    queue_cv.wait(lck, [](){return !commands.empty() || done;});
    pair<char, int> command = commands.front();
    commands.pop();

    // Notify all waiting threads... this doesn't seem to be working
    if (commands.empty() && done) {
        queue_cv.notify_all();
    }

    lck.unlock();
    return command;
}

// // Safe get 2
// pair<char, int> safeGet() {
//     unique_lock<mutex> lck(queue_mutex);

//     if (commands.empty() && done) {
//         return pair<char, int> ('D', '0');
//     }

//     queue_cv.wait(lck, [](){return !commands.empty() || done;});

//     // If we stopped waiting because producer is done
//     if (done) {
//         return pair<char, int> ('D', '0');
//     }

//     pair<char, int> command = commands.front();
//     commands.pop();
//     lck.unlock();
//     return command;
// }

void safeLog(string logfile, float time, int id, string event, int q, int n) {

    // Get a lock and write to log file
    unique_lock<mutex> lck(log_mutex);
    // log_cv.wait(lck);
    writeLog(logfile, time, id, event, q, n);

    // Update the stats object
    globalStats[event]++;
    if (event == "Complete") {
        threadStats[id]++;
    }
}
// Consumer 1
void consumer(int id) {
    
    while (!done || !commands.empty()) {  // Consumers keep going until producer is done and queue is empty

        safeLog(logfile, getElapsed(start), id, "LOOPING", -1, -1);

        safeLog(logfile, getElapsed(start), id, "Ask", -1, -1);
        pair<char, int> command = safeGet();
        printf("Thread %d - Got %c%d\n", id, command.first, command.second);
        safeLog(logfile, getElapsed(start), id, "Receive", commands.size(), command.second);
        if (command.first == 'T') {
            Trans(command.second);
            safeLog(logfile, getElapsed(start), id, "Complete", -1, command.second);
        } else {
            throw logic_error("Consumer got work that wasn't T! This should never happen");
        }

    }

    printf("Thread %d - Done.\n", id);
}

// Consumer 2
// void consumer(int id) {
    
//     pair<char, int> command = safeGet();
//     safeLog(logfile, getElapsed(start), id, "Ask", -1, -1);

//     while (!done || !commands.empty()) {  // Consumers keep going until producer is done and queue is empty

//         printf("Thread %d - Got %c%d\n", id, command.first, command.second);
//         safeLog(logfile, getElapsed(start), id, "Receive", commands.size(), command.second);
//         if (command.first == 'T') {
//             Trans(command.second);
//             safeLog(logfile, getElapsed(start), id, "Complete", -1, command.second);
//         } else if (command.first == 'D') {
//             break;
//         } else {
//             throw logic_error("Consumer got work that wasn't T! This should never happen");
//         }
//         safeLog(logfile, getElapsed(start), id, "Ask", -1, -1);
//         command = safeGet();
//     }
// }

int main(int argc, char **argv) {

    // Save start time
    int nthreads;
    const char *filenum;
    int id = 0;

    // Determine nthreads and id
    if (argc == 2) {
        // Set ID to default value
        nthreads = strtol(argv[1], nullptr, 0);
        filenum = 0;
    } else { // TODO: Something has to change here
        // Both arguements passed in
        nthreads = strtol(argv[1], nullptr, 0);
        filenum = (char *) argv[2];
    }// } else {
    //     // Print error and exit if invalid arguement count
    //     printf("Invalid argument count\n");
    //     exit(1);
    // }
    logfile = "prodcon." + (string) filenum + ".log";
    printf("nthreads: %d, filenum: %s\n", nthreads, filenum);

    // Setup global and thread stats
    globalStats["Work"] = 0;
    globalStats["Ask"] = 0;
    globalStats["Receive"] = 0;
    globalStats["Complete"] = 0;
    globalStats["Sleep"] = 0;
    for (int i = 1; i < nthreads + 1; i++) { 
        // Did this in it's own loop instead of below to prevent race condition
        threadStats[i] = 0;
    }

    // Create thread variables (should do this as a vector eventually)
    vector<thread> threads;

    // Spawn consumer threads
    for (int i = 1; i < nthreads + 1; i++) { // Start at 1 so each consumer has ID > 0
        threads.push_back(thread(consumer, i));
    }

    /* Producer loop */
    for (string line; getline(cin, line);) {
        
        // Split into pair of first and second character, converting second character to an integer
        pair<char, int> cmd (line[0], line[1] - '0');

        // Producer waits when queue is full - should change this to use CV/lock?
        while (commands.size() >= 2*nthreads) {}

        // Producer adds work to queue
        if (cmd.first == 'T') {
            safeAdd(cmd);
            safeLog(logfile, getElapsed(start), id, "Work", commands.size(), cmd.second);
        } else {
            Sleep(cmd.second);
            safeLog(logfile, getElapsed(start), id, "Sleep", -1, cmd.second);
        }
    }

    // Producer done
    done = true;
    safeLog(logfile, getElapsed(start), id, "End", -1, -1);

    // Wait for threads to complete once producer loop is done
    for (thread &t : threads) {
        if (t.joinable()) {
            t.join();
        }
    }

    // Write the stats summary
    writeLogStats(logfile, globalStats, threadStats);

    exit(0);
}

/** ISSUES
 * - Program will not end sometimes, seems more likely with more threads
 */