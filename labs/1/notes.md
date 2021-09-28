# Lab 1

## Creating archives

`tar -cf archivename.tar file1.cpp file2.h file3.txt`

## Compilation and makefiles

```makefile
# Declare variables in makefile

CC=gcc

CFLAGS= -c -Wall

# To compile seperately, e.g. with objects, have your main target like this:

hello: main.o function1.o function2.o
    $(CC) main.o function.0 function2.o -o hello

# Then each part compiles sperately to create object (.o) files like this:

main.o: main.c
    $(CC) $(CFLAGS) -c main.c

function1.o: function1.c
    $(CC) $(CFLAGS) -c function1.c

function2.o: function2.c
    $(CC) $(CFLAGS) -c function2.c

# Remove all intermediate files

clean:
    rm -rf *.o hello
```

## GBD

- Run a file in GBD: `GBD <./executable_name>`
- Create breakpoint: `b <line_number>`
- Display all breakpoints: `info b`
- Delete breakpoint: `delete <breakpoint_num>`
- Get values of variables in current scope: `info locals`
- Print value of a specific variable: `print <variable_name>`
- To move to next line: `n`
- Get line you are current at: `f`
- Step in: `step`

## Valgrind

Used to find memory leaks in your code

Must compile code with the -g flag, e.g. `gcc -g program.c`

Then run `valgrind --leak-check program.c`

## Man command

Use `man <command_name>` to bring up the manual page for that command, e.g. `man valgrind`

Use `man -f printf` to find where in the manual pages the printf documentation lies