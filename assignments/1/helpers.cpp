#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <stdio.h>
#include <string.h>

using namespace std;

vector<string> split(char *s, char const *delimeter) {
    vector<string> tokens;
    char* token = strtok(s, delimeter);
    while (token != NULL) {
        tokens.push_back(token);
        token = strtok(NULL, delimeter);
    }
    return tokens;
}

string join(vector<string> strings) {
    string joined = "";
    for (auto s : strings) {
        joined.append(s);
        joined.append(" ");
    }
    return joined;
}

string checkStartSplit(vector<string> tokens, char c) {
    string s;
    for (auto token : tokens) {
        if (token[0] == c) {
            s = token.substr(1, string::npos);
        }
    }
    return s;
}

vector<char*> strVec2CharVec(vector<string> v) {
    
    string *s = &v[0];

    // Try converting vector or strings to char **
    vector<char*> cV;
    cV.reserve(v.size());

    for (size_t i = 0; i < v.size(); ++i) {
        cV.push_back(const_cast<char*>(v[i].c_str()));
    }

    return cV;
}

/* Tester function for evaluating a status pointer from a waitpid call. Macro explanations below:

If status is not NULL, wait() and waitpid() store status information in the int to which it points. This integer can be inspected with the following macros (which take the integer itself as an argument, not a pointer to it, as is done in wait() and waitpid()!):

WIFEXITED(status)
    returns true if the child terminated normally, that is, by calling exit(3) or _exit(2), or by returning from main().
WEXITSTATUS(status)
    returns the exit status of the child. This consists of the least significant 8 bits of the status argument that the child specified in a call to exit(3) or _exit(2) or as the argument for a return statement in main(). This macro should only be employed if WIFEXITED returned true.
WIFSIGNALED(status)
    returns true if the child process was terminated by a signal.
WTERMSIG(status)
    returns the number of the signal that caused the child process to terminate. This macro should only be employed if WIFSIGNALED returned true.
WCOREDUMP(status)
    returns true if the child produced a core dump. This macro should only be employed if WIFSIGNALED returned true. This macro is not specified in POSIX.1-2001 and is not available on some UNIX implementations (e.g., AIX, SunOS). Only use this enclosed in #ifdef WCOREDUMP ... #endif.
WIFSTOPPED(status)
    returns true if the child process was stopped by delivery of a signal; this is only possible if the call was done using WUNTRACED or when the child is being traced (see ptrace(2)).
WSTOPSIG(status)
    returns the number of the signal which caused the child to stop. This macro should only be employed if WIFSTOPPED returned true.
WIFCONTINUED(status)
    (since Linux 2.6.10) returns true if the child process was resumed by delivery of SIGCONT.
*/
void inspectStatus(int status) {
    cout << "WIFEXITED: " << WIFEXITED(status) << " | ";
    cout << "WEXITSTATUS: " << WEXITSTATUS(status) << " | ";
    cout << "WIFSIGNALED: " << WIFSIGNALED(status) << " | ";
    cout << "WTERMSIG: " << WTERMSIG(status) << " | ";
    cout << "WIFSTOPPED: " << WIFSTOPPED(status) << " | ";
    cout << "WSTOPSIG: " << WSTOPSIG(status) << " | ";
    cout << "WIFCONTINUED: " << WIFCONTINUED(status) << endl;
}

// // FOR TESTING - uncomment this and run "g++ helpers.cpp" then "./a.out"
// int main() {
//     vector<string> t;
//     string s1;
//     string s2;
//     s1 = "string1";
//     s2 = "string2";
//     t.push_back(s1);
//     t.push_back(s2);
//     cout << join(t) << endl;

//     return 1;
// }