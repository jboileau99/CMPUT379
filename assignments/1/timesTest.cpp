#define _POSIX_SOURCE
#include <cstdio>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <sys/time.h>
#include <sys/resource.h>

using namespace std;

int main() {
  int status;
  long i, j;
  struct tms t;
  clock_t dub;

  int tics_per_second;

  tics_per_second = sysconf(_SC_CLK_TCK);
  struct rusage r;
  struct timeval start, end;
  getrusage(RUSAGE_CHILDREN, &r);
  start = r.ru_stime;

  if (fork() == 0) {
    int i = 0;
    int j = 0;
    int count_to = 1000000000; // Takes like 1/4 second
    int multiplier = 2; // for multiplier*(1/4) seconds total

    while (j < multiplier) {
        i = 0;
        while (i < count_to) {
            i++;
        }
        j++;
    }
    exit(0);
  }

  if (wait(&status) == -1)
    perror("wait() error");
  else if (!WIFEXITED(status))
    puts("Child did not exit successfully");
  else if ((dub = times(&t)) == -1)
    perror("times() error");
  else {
    // printf("process was dubbed %f seconds ago.\n\n",
        //    ((double) dub)/tics_per_second);
    printf("            utime           stime\n");
    printf("parent:    %f        %f\n",
           ((double) t.tms_utime)/tics_per_second,
           ((double) t.tms_stime)/tics_per_second);
    printf("child:     %f        %f\n",
           ((double) t.tms_cutime)/tics_per_second,
           ((double) t.tms_cstime)/tics_per_second);
    getrusage(RUSAGE_CHILDREN, &r);
    end = r.ru_utime;
    printf("Start: %ld", start.tv_sec);
    printf("End: %ld", end.tv_sec);
  }

    //cout << "User time: " << r.ru_utime << endl;
    //cout << "System time: " << r.ru_stime << endl;

  return 1;
}