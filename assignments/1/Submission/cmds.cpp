/**
 * CMPUT 379 Assignment 1
 * Justin Boileau | 1535268
 * 
 * System call related commands (most of them) defined here
 * 
 **/

#include "data.h"
#include "cmds.h"
#include "helpers.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <thread>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <error.h>
#include <sys/times.h>

/**
 * Shell commands
 */

int _exit(vector<string> args) {
    pid_t pid;
    int status;

    // Wait for all processes to finish and then update user and sys times
    while ((pid = wait(&status)) > 0);
    updateTimes();

    // Print user and sys time summary
    cout << "\nResources used" << endl;
    cout << "User time =" << right << setw(6) << setfill(' ') << total_utime << " seconds" << endl;
    cout << "Sys  time =" << right << setw(6) << setfill(' ') << total_stime << " seconds" << endl << endl;
    
    exit(0);
    return 0;
}

int _jobs(vector<string> args) {

    // Get updated timing info for each process - TEST THIS TMRW
    getUpdatedProcessTimes();

    // Print formatted output table
    cout << "\nRunning processes:" << endl;
    if (!process_table.empty()) {
        cout << " " << "#" << setw(6) << setfill(' ') << " " << "PID S SEC COMMAND" << endl;
        for (auto it = process_table.begin(); it != process_table.end(); it++) {
            
            // Ensure proper widths
            cout << " " << distance(begin(process_table), it) << ":" << right << setw(8) << setfill(' ') << it->first << " " << setw(1) << it->second.status << " " << right << setw(3) << setfill(' ') << it->second.runtime << " " << left << setw(30) << setfill(' ') << it->second.command << endl;

        }
    }

    cout << "Processes =" << right << setw(6) << setfill(' ') << process_table.size() << " active" << endl;
    cout << "Completed processes:" << endl;
    cout << "User time =" << right << setw(6) << setfill(' ') << total_utime << " seconds" << endl;
    cout << "Sys  time =" << right << setw(6) << setfill(' ') << total_stime << " seconds" << endl << endl;

    return 0;
}

int _kill(vector<string> args) {
    // Grab pid from arg tokens and send SIGKILL
    try {
        int pid = stoi(args[1]);
        kill(pid, SIGKILL);
    } catch (exception&) {
        return -1;
    }
    return 0;
}

int _resume(vector<string> args) {
    // Grab pid from arg tokens and send SIGCONT
    try {
        int pid = stoi(args[1]);
        kill(pid, SIGCONT);
    } catch (exception&) {
        return -1;
    }
    return 0;
}

int _sleep(vector<string> args) {
    // Grab duration from arg tokens and pass to sleep command
    try {
        if (args.size() < 2) {return -1;}
        int duration = stoi(args[1]);
        sleep(duration);
    } catch (exception&) {
        return -1;
    }
    return 0;
}

int _suspend(vector<string> args) {

    // Grab pid from arg tokens and send SIGSTOP
    try {
        int pid;
        pid = stoi(args[1]);
        kill(pid, SIGSTOP);
    } catch (exception& e) {
        return -1;
    }

    return 0;
}

int _wait(vector<string> args) {
    // Grab pid from arg tokens and wait
    try {
        int status;
        int pid = stoi(args[1]);
        waitpid(pid, &status, 0);
        handleStatus(status, pid);
    } catch (exception&) {
        return -1;
    }
    return 0;
}

int _exec(vector<string> args) {

    // Convert string vector to char vector and add a NULL terminater required by execv family
    vector<char*> cArgs = strVec2CharVec(args);
    cArgs.push_back(NULL);

    try {
        if (execvp(cArgs[0], &cArgs[0])) {
            perror(("Error running '" + args[0] + "'").c_str());
        };
    // If code gets past here, something is wrong as exec should take over process
    } catch (exception&) {
        return -1;
    }
    return -1;
}

/**
 *  Process Table Functions
 */

bool addProcess(int pid, char status, string command) {
    Process p = { status, 0, command};
    process_table[pid] = p;

    return true; // should do some error checking to see if process was added and return t/f?
}

bool updateProcessStatus(int pid, char newStatus) {
    // Get iterator to element with key pid
    auto p = process_table.find(pid);
    if (p != process_table.end()) {
        // Key valid, update value
        p->second.status = newStatus;
        return true;
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
        return true;
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
        return true;
    } else {
        // Invalid key, i.e. pid not in process table
        return false;
    }
}

void getUpdatedProcessTimes() {

    // Use pipe to read ps output
    FILE * p;
    char path[2048];
    p = popen("ps", "r");
    vector<string> psLine;
    int pid, h, m, s, seconds;

    while (fgets(path, 2048, p) != NULL) {

        // Split into vector of std::strings
        psLine = split(path, (char*)" ");

        // Skip any non-process lines, e.g. the header line
        if (psLine[0] == "PID") {
            continue;
        }

        // Convert string pid to int
        pid = atoi(psLine[0].c_str());

        // Parse seconds from time
        if (sscanf(psLine[2].c_str(), "%d:%d:%d", &h, &m, &s) >= 2) {
            seconds = h*3600 + m*60 + s;
        }

        // Update table
        updateProcessTime(pid, seconds);
    }
    pclose( p );
}

void updateTimes() {
    static struct tms st_cpu;
    times(&st_cpu);
    total_utime = st_cpu.tms_cutime/sysconf(_SC_CLK_TCK);
    total_stime = st_cpu.tms_cstime/sysconf(_SC_CLK_TCK);
}

/**
 * Signal Callbacks
 */

void handleStatus(int status, int pid) {

    if (pid == -1) {
        // Error with waitpid
        //cout << "SIGCHLD from already waited process, or error with waitpid" << endl;
    } else if (pid == 0) {
        // Still running... set status to R?
        //cout << pid << " still running" << endl;
    } else {
        // Determine process state
        if (WIFEXITED(status)) {
            //cout << " terminated normally";
            // Update sys and user times
            updateTimes();
            // Remove terminated process from table
            removeProcessPID(pid);
        } else if (WIFSIGNALED(status)) {
            //cout << " terminated by signal";
            // Update sys and user times
            updateTimes();
            // Remove terminated process from table
            removeProcessPID(pid);
        } else if (WIFSTOPPED(status)) {
            //cout << " suspended by signal";
            updateProcessStatus(pid, PSTATUS_SUSPENDED);
        } else if (WIFCONTINUED(status)) {
            //cout << " resumed by signal";
            updateProcessStatus(pid, PSTATUS_RUNNING);
        } else {
            //cout << " undetermined";
        }
        //cout << endl;
    }
}

void SIGCHLDCallback(int signum) {
    int status;
    int pid = waitpid(-1, &status, WNOHANG + WUNTRACED);
    handleStatus(status, pid);

    //cout << "Got sigchld from PID: " << pid << " Status: " << status << endl;
    //inspectStatus(status);
}