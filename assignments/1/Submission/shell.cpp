/**
 * CMPUT 379 Assignment 1
 * Justin Boileau | 1535268
 * 
 * Main shell program for getting input, parsing, and dispatching commands.
 * 
 **/

#include "data.h"
#include "shell.h"
#include "helpers.h"

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <list>
#include <typeinfo>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>

// Global variables
map<int, Process> process_table;
intmax_t total_utime;
intmax_t total_stime;

void initShell() { 
    cout << SHELL_GREETING << endl;
}


vector<string> getInput() {

    // Print out shell prefix and get 100 characters of input
    char buf[LINE_LENGTH];
    cout << SHELL_PREFIX;
    cin.getline(buf, LINE_LENGTH);

    // Split into space delimited tokens
    vector<string> tokens = split(buf, (char*)" ");
    
    return tokens;
}

void dispatchCmd(vector<string> args, string inFile, string outFile, bool externalCmd, bool background) {

    // Initialize vars
    int pid = getpid();
    int result, status;

    // Get the command name
    string cmd = args[0];

    // Spawn child proccess if this command is to be run in background
    if (background || externalCmd) {
        pid = fork();
    }

    // Process in foreground or this is the child
    if ((!background && !externalCmd) || pid == 0) {

        // Save previous stdout so we can restore it
        int prev_stdout = dup(1);
        if (prev_stdout < 0) {
            perror("dup failed!");
        }

        // Setup input piping if provided
        if (!inFile.empty()) {

            // Open infile and get file descriptor
            int ffd;
            if ((ffd = open(inFile.c_str(), O_CREAT | O_RDWR, 0644)) < 0) {
                perror("open failed!");
            }

            // Point stdin to inFile
            if (dup2(ffd, STDIN_FILENO) < 0) {
                perror("dup2 failed!");
            }
            close(ffd);

            // Remove <FILE so it isn't treated as an arguement
            args = removeElement(args, "<" + inFile);
        }

        // Setup output piping if provided
        if (!outFile.empty()) {
            
            // Open outfile and get file descriptor
            int ffd;
            if ((ffd = open(outFile.c_str(), O_CREAT | O_RDWR, 0644)) < 0) {
                perror("open failed!");
            }
            
            // Point stdout to outFile
            if (dup2(ffd, STDOUT_FILENO) < 0) {
                perror("dup2 failed!");
            }
            close(ffd);

            // Remove >FILE so it isn't treated as an arguement
            args = removeElement(args, ">" + outFile);
        }

        // Run the called shell command or execute some other program
        if (cmd == "exit") {
            result = _exit(args);
        } else if (cmd == "jobs") {
            result = _jobs(args);
        } else if (cmd == "kill") {
            result = _kill(args);
        } else if (cmd == "resume") {
            result = _resume(args);
        } else if (cmd == "sleep") {
            result = _sleep(args);
        } else if (cmd == "suspend") {
            result = _suspend(args);
        } else if (cmd == "wait") {
            result = _wait(args);
        } else {
            result = _exec(args);
        }

        // Print error message if error code returned
        if (result != 0) {
            printf("Error running %s: %s", cmd.c_str(), ERROR_MAP.find(result)->second.c_str());
        }

        // Restore stdout to shell
        if (pid != 0) {
            dup2(prev_stdout, 1);
            close(prev_stdout);
        } else {
        // Child processes should exit after completing
            exit(0);
        }

    } else {
        // Parent - add child to process table
        if (cmd == "sleep") {
            status = PSTATUS_SLEEPING;
        } else {
            status = PSTATUS_RUNNING;
        }
        addProcess(pid, status, join(args));

        // Wait for process spawned by exec command if we didn't get the background (&) flag
        if (externalCmd && !background) {
            int status;            
            waitpid(pid, &status, 0);
            
            // Pass status and PID to function that will update process table
            handleStatus(status, pid);
        }
    }
}

int main() {

    // Will want to init process table here too
    initShell();

    // Initialize variables
    vector<string> tokens;
    string inFile;
    string outFile;
    bool externalCmd;
    bool background;

    // Sig handlers
    signal(SIGCHLD, SIGCHLDCallback);

    // Repeatedly get input
    while (true) {

        // Get a vector of space seperated tokens
        tokens = getInput();

        // Process and dispatch if input not empty
        if (!tokens.empty()) {

            // Check for the 3 possible command modifiers or if this is a non-shell379 command
            string inFile = checkStartSplit(tokens, INFILE_CHAR);
            string outFile = checkStartSplit(tokens, OUTFILE_CHAR);
            externalCmd = find(SHELL_CMDS.begin(), SHELL_CMDS.end(), tokens.front()) == SHELL_CMDS.end();
            background = tokens.back()[0] == BACKGROUND_CHAR;

            // Run the command
            dispatchCmd(tokens, inFile, outFile, externalCmd, background);
        }
    }
    return 0;
}