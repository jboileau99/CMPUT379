/**
 * CMPUT 379 Assignment 1
 * Justin Boileau | 1535268
 * 
 * Global constants, error flags, process struct and table defined here.
 * 
 **/

#include "cmds.h"

#include <map>
#include <functional>
#include <string>
#include <vector>

using namespace std;

// Assignment constants
const int LINE_LENGTH = 100;    // Max # of characters in an input line
const int MAX_ARGS = 7;         // Max number of arguments to a command
const int MAX_LENGTH = 20;      // Max # of characters in an argument
const int MAX_PT_ENTRIES = 32;  // Max entries in the Process Table

// Shell strings
const string SHELL_PREFIX = "SHELL379: ";
const string SHELL_GREETING = "*************************\n*                       *\n*  Welcome to shell379! *\n*                       *\n*************************";

// Input modifiers
const char INFILE_CHAR = '<';
const char OUTFILE_CHAR = '>';
const char BACKGROUND_CHAR = '&';

// Status constants
const char PSTATUS_RUNNING = 'R';
const char PSTATUS_SLEEPING = 'S';
const char PSTATUS_SUSPENDED = 'T';

// Possible shell cmds
const vector<string> SHELL_CMDS = {"exit", "jobs", "kill", "resume", "sleep", "suspend", "wait"};

// Error numbers and string mappings
const int CMD_UNDEFINED_ERROR = -1;
const int CMD_NOT_ENOUGH_ARGS_ERROR = -2;
const int CMD_NON_INTEGER_ARG_ERROR = -3;
const int CMD_NO_EXEC_ERROR = -4;
const int PTABLE_PID_NOT_FOUND_ERROR = -5;

const map<int, string> ERROR_MAP = {
    {CMD_UNDEFINED_ERROR, "Undefined error."},
    {CMD_NOT_ENOUGH_ARGS_ERROR, "Not enough arguements."},
    {CMD_NON_INTEGER_ARG_ERROR, "Arguement is not an integer."},
    {CMD_NO_EXEC_ERROR, "Exec command did not run."},
    {PTABLE_PID_NOT_FOUND_ERROR, "PID not in table."}
};

// Process table
struct Process {
    char status;
    int runtime;
    string command;
};
extern map<int, Process> process_table;

// Timing variables
extern intmax_t total_utime;
extern intmax_t total_stime;