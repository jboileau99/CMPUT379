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

// Global variables
// map<string, pfunc> func_map;
map<int, Process> process_table;

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

// Determine the command and arguements from given input
void dispatchCmd(vector<string> args, string inFile, string outFile, bool externalCmd, bool background) {

    // Initialize vars
    int pid = getpid();

    // Get the command name
    string cmd = args[0];

    // Setup pipe here, before fork
    // https://stackoverflow.com/questions/7292642/grabbing-output-from-exec
    // http://www.gnu.org/savannah-checkouts/gnu/libc/manual/html_node/Creating-a-Pipe.html
    // Maybe should only do this if background or externalCmd? aka we're going to fork
    int link[2];
    pipe(link);

    // Spawn child proccess if this command is to be run in background
    if (background || externalCmd) {
        pid = fork();
    }

    // Process in foreground or this is the child
    if ((!background && !externalCmd) || pid == 0) {

        if (cmd == "exit") {
            sleep(10);
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
        addProcess(pid, PSTATUS_RUNNING, join(args));

        if (externalCmd && !background) {
            int status;
            waitpid(pid, &status, 0);
        }
    }
}

int main() {

    /**
     * Issue
     * 
     * I thought suspend/resume/sleep wasn't working right...
     * 
     * But actually it's cause the sleep() function runs on real-time seconds, so I don't think stopped processes count
     *
     * So maybe we're ok?
     * 
     * TODO
     * - Exec command
     * - Pipe implementation
     * - Clean up processes on _exit() command
     */

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