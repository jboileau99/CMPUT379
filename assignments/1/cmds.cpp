#include "data.h"
#include "cmds.h"
#include "helpers.h"

#include <iostream>
#include <thread>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * Shell commands
 */

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
        for (auto it = process_table.begin(); it != process_table.end(); it++) {
            cout << " " << it->second.number << ": " << it->first << " " << it->second.status << "   " << it->second.runtime << " " << it->second.command << endl;
        }
    }

    cout << "Processes =\t" << process_table.size() << " active" << endl; // Are active processes ones with status = R or any not terminated?
    cout << "Completed processes:" << endl;
    cout << "User time =\t" << "TODO seconds" << endl;
    cout << "Sys  time =\t" << "TODO seconds" << endl;

    return 1;
}

int _kill(vector<string> args) {
    // Grab pid from arg tokens and send SIGKILL
    try {
        int pid = stoi(args[1]);
        kill(pid, SIGKILL);
    } catch (exception&) {
        // TODO: Do something with errors codes once this is all working
        return -1;
    }

    // Update status in process table, or should main thread do this?

    return 1;
}

int _resume(vector<string> args) {
    // Grab pid from arg tokens and send SIGCONT
    try {
        int pid = stoi(args[1]);
        kill(pid, SIGCONT);
    } catch (exception&) {
        // TODO: Do something with errors codes once this is all working
        return -1;
    }

    // Update status in process table, or should main thread do this?

    return 1;
}

int _sleep(vector<string> args) {
    // Grab duration from arg tokens and pass to sleep command
    try {
        int duration = stoi(args[1]);
        sleep(duration);
    } catch (exception&) {
        // TODO: Do something with errors codes once this is all working
        return -1;
    }
    return 1;
}

int _suspend(vector<string> args) {

    // Grab pid from arg tokens and send SIGSTOP
    try {
        int pid = stoi(args[1]);
        kill(pid, SIGSTOP);
    } catch (exception&) {
        // TODO: Do something with errors codes once this is all working
        return -1;
    }

    // Update status in process table, or should main thread do this?

    return 1;
}

int _wait(vector<string> args) {
    // Grab pid from arg tokens and send SIGSTOP
    int status;
    try {
        int pid = stoi(args[1]);
        waitpid(pid, &status, 0);
    } catch (exception&) {
        // TODO: Do something with errors codes once this is all working
        return -1;
    }
    return 1;
}

int _exec(vector<string> args) {
    /**
     * This works:
     *      char *a[] = {"ls_test", NULL};
     *      execv(a[0], a);
     */

    vector<char*> cArgs = strVec2CharVec(args);

    try {
        execv(cArgs[0], &cArgs[0]);

        // Pipe output here

    } catch (exception&) {
        // TODO: Do something with errors codes once this is all working
        return -1;
    }
    return 1;
}

/**
 *  Process Table Functions
 */

bool addProcess(int pid, char status, string command) {
    Process p = {process_table.size(), status, 0, command};
    process_table[pid] = p;
    if (debug) {
        cout << "added process: " << pid << " " << p.status << " " << p.command << endl;
    }

    return true; // should do some error checking to see if process was added and return t/f?
}

/**
The status of the task which can be one of:
    D = uninterruptible sleep
    I = idle
    R = running
    S = sleeping
    T = stopped by job control signal
*/
bool updateProcessStatus(int pid, char newStatus) {
    // Get iterator to element with key pid
    auto p = process_table.find(pid);
    if (p != process_table.end()) {
        // Key valid, update value
        p->second.status = newStatus;
    } else {
        // Invalid key, i.e. pid not in process table
        return false;
    }
}

bool updateProcessTime(int pid, int newTime) {
    // Get iterator to element with key pid
    auto p = process_table.find(pid);
    if (p != process_table.end()) {
        // Key valid, update value
        p->second.runtime = newTime;
    } else {
        // Invalid key, i.e. pid not in process table
        return false;
    }
}

bool removeProcessPID(int pid) {

    // Get iterator to element with key pid
    auto p = process_table.find(pid);
    if (p != process_table.end()) {
        // Key valid
        process_table.erase(p);
    } else {
        // Invalid key, i.e. pid not in process table
        return false;
    }
}

/**
 * Signal Callbacks
 */

void SIGCHLDCallback(int signum) {
    int status;
    int pid = waitpid(-1, &status, WNOHANG + WUNTRACED);

    if (pid == -1) {
        // Error with waitpid
        //cout << "SIGCHLD from already waited process, or error with waitpid" << endl;
    } else if (pid == 0) {
        // Still running... set status to R?
        cout << pid << " still running" << endl;
    } else {
        // Determine process state
        cout << pid << " ";
        if (WIFEXITED(status)) {
            cout << " terminated normally";
            removeProcessPID(pid);
        } else if (WIFSIGNALED(status)) {
            cout << " terminated by signal";
            removeProcessPID(pid);
        } else if (WIFSTOPPED(status)) {
            cout << " suspended by signal";
            updateProcessStatus(pid, PSTATUS_SUSPENDED);
        } else if (WIFCONTINUED(status)) {
            cout << " resumed by signal";
            updateProcessStatus(pid, PSTATUS_RUNNING);
        } else {
            cout << "something else happened...";
        }
        cout << endl;
    }
    //cout << "Got sigchld from PID: " << pid << " Status: " << status << endl;
    //inspectStatus(status);
}