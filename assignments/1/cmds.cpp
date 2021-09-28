#include "cmds.h"
#include "data.h"

#include <unistd.h>
#include <iostream>
#include <thread>
#include <chrono>

int _exit(vector<string> args) {
    cout << "EXITING" << endl;
    return 1;
}

int _jobs(vector<string> args) {
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
    return 1;
}

int _wait(vector<string> args) {
    return 1;
}

int _exec(vector<string> args) {
    return 1;
}