/**
 * CMPUT 379 Assignment 1
 * Justin Boileau | 1535268
 * 
 * System call related commands (most of them) defined here.
 * 
 **/

#include <vector>
#include <string>

using namespace std;

#ifndef CMD_FUNCTIONS
#define CMD_FUNCTIONS

// Shell commands

// Wait for all processes to finish then print times summary and exit
int _exit(vector<string> args);

// Get process info and times summary
int _jobs(vector<string> args);

// Kill process PID
int _kill(vector<string> args);

// Resume process PID
int _resume(vector<string> args);

// Sleep for x seconds
int _sleep(vector<string> args);

// Suspend process PID
int _suspend(vector<string> args);

// Wait for process PID to finish
int _wait(vector<string> args);

// Executes some other file/command
int _exec(vector<string> args);

// Process table commands

// Add new process with given values to table
bool addProcess(int pid, char state, string command);

// Update status for process with input PID
// The status of the task which can be one of:
//     I = idle
//     R = running
//     S = sleeping
//     T = stopped by job control signal
bool updateProcessStatus(int pid, char newStatus);

// Update execution time for process with input PID
bool updateProcessTime(int pid, int newTime);

// Remove a process with input PID
bool removeProcessPID(int pid);

// Get updated process times by piping 'ps' command
void getUpdatedProcessTimes();

// Update user and sys times for terminated processes
void updateTimes();

// Determine what happened to a child process based on the status value and update process table accordingly
void handleStatus(int status, int pid);

// Signal callbacks
void SIGCHLDCallback(int signum);

#endif