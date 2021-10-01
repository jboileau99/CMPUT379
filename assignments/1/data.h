#include "cmds.h"

#include <map>
#include <functional>
#include <string>
#include <vector>

using namespace std;
using pfunc = int (*)(vector<string>);

const int LINE_LENGTH = 100;    // Max # of characters in an input line
const int MAX_ARGS = 7;         // Max number of arguments to a command
const int MAX_LENGTH = 20;      // Max # of characters in an argument
const int MAX_PT_ENTRIES = 32;  // Max entries in the Process Table
const string SHELL_PREFIX = "SHELL379: ";
const string SHELL_GREETING = "*************************\n*                       *\n*  Welcome to shell379! *\n*                       *\n*************************";
const char INFILE_CHAR = '<';
const char OUTFILE_CHAR = '>';
const char BACKGROUND_CHAR = '&';
const char PSTATUS_RUNNING = 'R';
const char PSTATUS_SLEEPING = 'S';
const char PSTATUS_SUSPENDED = 'T';

const vector<string> SHELL_CMDS = {"exit", "jobs", "kill", "resume", "sleep", "suspend", "wait"};

struct Process {
    int number;
    char status;
    int runtime;
    string command;
};

extern map<int, Process> process_table;
extern bool debug;