#include "shell.h"
#include "data.h"
#include "helpers.h"

#include <iostream>
#include <map>
#include <vector>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

// Global variables
// map<string, pfunc> func_map;
// map<string, Process> process_table;

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
void dispatchCmd(vector<string> args, string inFile, string outFile, bool background) {

    // Get the command name
    string cmd = args[0];

    // Fork to new process if not & (background)
    int pid = fork();

    // Child
    if (pid == 0) {    
        if (cmd == "exit") {
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

    // Parent
    } else {
        waitpid(pid, NULL, 0);
    }
}


int main() {

    // Will want to init process table here too
    initShell();

    // Repeatedly get input
    vector<string> tokens;
    while (true) {
        tokens = getInput();
        //bool background = tokens.back() == BACKGROUND_CHAR;
        string infile = checkStartSplit(tokens, "<");
        string outfile = checkStartSplit(tokens, ">");
        cout << " " << infile << " " << outfile << endl;
        //dispatchCmd(tokens);
    }

    return 0;
}