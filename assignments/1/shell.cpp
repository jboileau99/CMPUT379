#include "data.h"
#include "shell.h"
#include "helpers.h"

#include <iostream>
#include <map>
#include <vector>
#include <list>
#include <typeinfo>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

// Global variables
// map<string, pfunc> func_map;
vector<Process> process_table;

// Can delete this after
bool debug = false;

void initShell() {
    
    cout << SHELL_GREETING << endl;
}

// Gets a line from the shell, returns vector of space delimited tokens
vector<string> getInput() {

    // Print out shell prefix and get 100 characters of input
    char buf[LINE_LENGTH];
    cout << SHELL_PREFIX;
    cin.getline(buf, LINE_LENGTH);

    // Split into space delimited tokens
    vector<string> tokens = split(buf, (char*)" ");
    
    return tokens;
}

bool addProcess(int pid, char status, string command) {
    Process p = {pid, status, 0, command};
    process_table.push_back(p);
    if (debug) {
        cout << "added process: " << p.pid << " " << p.status << " " << p.command << endl;
    }

    return true; // should do some error checking to see if process was added and return t/f?
}

bool updateProcessStatus(int pid, char newStatus) {
    // TODO: change process table to map with PID keys?
}

bool updateProcessTime(int pid, int newTime) {

}

bool removeProcessNumber(long unsigned int num) {
    if (num >= 0 && num < process_table.size()) {
        // Erase process at position num
        process_table.erase(process_table.begin() + num);
        return true;
    }
    // Otherwise, Invalid process number
    return false;
}

bool removeProcessPID(int pid) {
    for (long unsigned int i = 0; i < process_table.size(); i++) {

        // Erase process and return sucess if found
        if (process_table[i].pid == pid) {
            process_table.erase(process_table.begin() + i);
            return true;
        }
    }
    // If we make it here we didn't find the PID, so return false
    return false;
}

// Determine the command and arguements from given input
void dispatchCmd(vector<string> args, string inFile, string outFile, bool background) {

    // Initialize vars
    int pid = getpid();

    // Get the command name
    string cmd = args[0];

    // Spawn child proccess if this command is to be run in background
    if (background) {
        pid = fork();
    }

    // Process in foreground or this is the child
    if (!background || pid == 0) {

        if (cmd == "exit") {
            sleep(3);
            _exit(args);
        } else if (cmd == "jobs") {
            _jobs(args);
        } else if (cmd == "kill") {
            _kill(args);
        } else if (cmd == "resume") {
            _resume(args);
        } else if (cmd == "sleep") {
            _sleep(args);
        } else if (cmd == "suspend") {
            _suspend(args);
        } else if (cmd == "wait") {
            _wait(args);
        } else {
            _exec(args);
        }

        // Child processes should exit after completing
        if (pid == 0) {
            exit(0);
        }

    } else {
        // Parent - add child to process table
        addProcess(pid, 'R', join(args));
    }
}

int main() {

    // Will want to init process table here too
    initShell();

    // Initialize variables
    vector<string> tokens;
    string inFile;
    string outFile;
    bool background;

    // Sig handlers
    signal(SIGCHLD, SIGCHLDCallback);

    // Repeatedly get input
    while (true) {

        // Get a vector of space seperated tokens
        tokens = getInput();
        if (!tokens.empty()) {

            // Check for the 3 possible command modifiers
            string inFile = checkStartSplit(tokens, INFILE_CHAR);
            string outFile = checkStartSplit(tokens, OUTFILE_CHAR);
            background = tokens.back()[0] == BACKGROUND_CHAR;

            // Run the command
            dispatchCmd(tokens, inFile, outFile, background);
        }
    }

    return 0;
}