#include "data.h"
#include "cmds.h"

#include <iostream>
#include <thread>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

int _exit(vector<string> args) {
    exit(0);

    // Clean up all processes - should be able to access the global process table here
    
    return 1;
}

int _jobs(vector<string> args) {

    // TODO: Probably a better way to format here...

    cout << "Running processes:" << endl;

    if (!process_table.empty()) {
        cout << " #     PID S SEC COMMAND" << endl;
        for (long unsigned int i=0; i < process_table.size(); i++) {
            cout << " " << i << ": " << process_table[i].pid << " " << process_table[i].status << "   " << process_table[i].runtime << " " << process_table[i].command << endl;
        }
    }

    cout << "Processes =\t" << process_table.size() << " active" << endl; // Are active processes ones with status = R or any not terminated?
    cout << "Completed processes:" << endl;
    cout << "User time =\t" << "TODO seconds" << endl;
    cout << "Sys  time =\t" << "TODO seconds" << endl;

    return 1;
}

int _kill(vector<string> args) {
    return 1;
}

int _resume(vector<string> args) {
    return 1;
}

int _sleep(vector<string> args) {
    return 1;
}

int _suspend(vector<string> args) {

    // Get pid from process table
    try {
        int pid = stoi(args[0]);
    } catch {
        // TODO: Do something with errors codes once this is all working
        return -1
    }

    kill(pid, SIGSTOP);

    // Update status in process table
    

    return 1;
}

int _wait(vector<string> args) {
    return 1;
}

int _exec(vector<string> args) {
    return 1;
}

void SIGCHLDCallback(int signum) {
    int status;
    int pid = waitpid(-1, &status, WNOHANG);
    cout << "Got sigchld from PID: " << pid << " Status: " << status << endl;
    cout << WIFEXITED(status) << endl;
}