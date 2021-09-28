#include "cmds.h"

#include <map>
#include <functional>
#include <string>

using namespace std;
using pfunc = int (*)(vector<string>);

const int LINE_LENGTH = 100;    // Max # of characters in an input line
const int MAX_ARGS = 7;         // Max number of arguments to a command
const int MAX_LENGTH = 20;      // Max # of characters in an argument
const int MAX_PT_ENTRIES = 32;  // Max entries in the Process Table
const string SHELL_PREFIX = "SHELL379: ";
const string SHELL_GREETING = "*************************\n*                       *\n*  Welcome to shell379! *\n*                       *\n*************************";
const char *INFILE_CHAR = "<";
const char *OUTFILE_CHAR = ">";
const char *BACKGROUND_CHAR = "&";

struct Process {
    int pid;
    int start_time;
    bool completed;
};

// {
//     {"exit", exit},
//     {"jobs", jobs},
//     {"kill", kill},
//     {"resume", resume},
//     {"sleep", sleep},
//     {"suspend", suspend},
//     {"wait", wait},
//     {"exec", exec},
// };