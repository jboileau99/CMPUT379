#include <cstdio>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    sleep(10);
    char *a[] = {"/bin/ls", NULL};
    execv(a[0], a);
}