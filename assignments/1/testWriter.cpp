#include <cstdio>
#include <iostream>
#include <thread>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

// int main() {
//     FILE *fp;
//     fp = fopen("example.txt","w");
//     char lang[5][20] = {"C","C++","Java","Python","PHP"};

//     fprintf(fp,"Top 5 programming language\n");
//     for (int i=0; i<5; i++)
//         fprintf(fp, "%d. %s\n", i+1, lang[i]);

//     fclose(fp);
//     return 0;
// }

// int main() {
//     // FILE *fp;
//     // fp = fopen("example.txt","w");
//     char lang[5][20] = {"C","C++","Java","Python","PHP"};

//     printf("Top 5 programming language\n");
//     for (int i=0; i<5; i++)
//         printf("%d. %s\n", i+1, lang[i]);

//     return 0;
// }

#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);

int main() {

    int link[2];
    char foo[4096];
    pipe(link);
    pid_t pid = fork();


    if (pid == 0) {
        // Child

        dup2 (link[1], STDOUT_FILENO);
        close(link[0]);
        close(link[1]);
        char *a[] = {"/bin/ls", NULL};
        execv(a[0], a);
        die("excel");

    } else {
        // Parent
        close(link[1]);
        int nbytes = read(link[0], foo, sizeof(foo));
        printf("Output: (%.*s)\n", nbytes, foo);
        wait(NULL);
    }

    return 0;

    // execl("/bin/ls", "ls", "-1", (char *)0);
    // int pipes[2];

    // pipe(pipes); // Create the pipes

    // dup2(pipes[1],1); // Set the pipe up to standard output

}