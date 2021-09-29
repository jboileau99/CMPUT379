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
bool debug = true;

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

// Determine the command and arguements from given input
void dispatchCmd(vector<string> args, string inFile, string outFile, bool background) {

    // Initialize vars
    int pid = getpid();

    // Get the command name
    string cmd = args[0];

    // Fork to new process if background flag set - OR should we just be calling wait here or lower if background is set?
    // pid = fork();
    // I think maybe the idea is only the exec command or background flag spawns a new process

    // Add the child to process table
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
        // if (pid == 0) {
        //     exit(0);
        // }

    } else {
        // Parent - add child to process table
        addProcess(pid, 'R', join(args));
    }
}

void sigchldCallback(int signum) {
    int status;
    int pid = waitpid(-1, &status, WNOHANG);
    cout << "Got sigchld from PID: " << pid << " Status: " << status << endl;
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
    signal(SIGCHLD, sigchldCallback);

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