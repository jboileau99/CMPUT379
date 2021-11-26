# CMPUT 379 Assignment 2

## Usage Instructions

Run "make" or "make prodcon" to generate a "prodcon" executable which can be run in the following form:
    "prodcon {nthreads} {id} {<inputFile}"

- Args:
    - nthreads (Mandatory): Number of threads to use when executing the program.
    - id (optional): Identifier for the logfile, e.g. id = 3 would lead to a logfile called "prodcon.3.log". If this arguement is empty the log file will be "prodcon.log"
    - inputFile (optional): Commands file to read from, otherwise will accept newline seperated commands from command line.

## Included files

### prodcon.cpp / prodcon.h

- Functions for adding and removing from a queue, writing to a log file, and keeping stats in a thread safe fashion.
- Consumer loop which is ran by all threads to ask for, wait for, and handle work
- Producer code to parse arguements, initialize statistics, spawn threads, and repeatedly add commands to the queue. Notifies threads when done.

### helpers.cpp / helpers.h

- Functions for writing consumer/producer status and statistics to log file, and getting elapsed time relative to program start

### tands.c

- Trans and sleep commands provided for assignment.

### Makefile

Targets:
- "make" or "make prodcon": compiles all cpp and header files, links together, and produces "prodcon" executable. Uses -O flag to optimize.
- "make clean": remove all intermediate .o files
- "make compress": used to generate .zip archive for assignment submission

## Assumptions
- Input is provided in the correct format (as stated in assignment)
- T and S work as intended
- nthreads can be contained within an int size variable

## Summary of Approach

- My approach was to identify areas of the problem where access to shared memory could present issues such as race conditions or garbled IO, and implement safe methods for doing these tasks using the synchronization options in c++. I used the C++ mutex, condition, and atomic variables.
- The 3 areas identified were:
  - Adding to commands queue
  - Getting from commands queue
  - Writing to log file
- To solve these I created 3 methods in prodcon.h/.cpp: safeLog, safeAdd, and safeGet
  - Each of these functions acquires a lock for its resource (logfile or queue) using a mutex variable, does its task, then returns the lock (leaves the critical section)
- The main two components to the program are the producer loop and the consumer loop
- Producer
  - Loop:
    - Read input from stdin
    - Call safeLog to record work received
    - Parse command and either execute a sleep or safeAdd a T command to the queue
  - If queue is full, producer will wait for a notification from consumer that work has been taken from queue and more can be added
  - Once complete producer will set an atomic done flag and notify any consumers currently waiting in safeGet

- Consumer
  - Loop:
    - Call safeLog to record an ask for work
    - Call safeGet to wait for then retrieve work
    - Process the command (call Trans) and call safeLog to record reception and completion of work
  - Consumers will wait in safeGet for notification from the producer that either:
    - Work was added
    - No more work (producer done)
  - Consumers notify producer after taking work, in case the producer was waiting because queue was full
  - Even when the producer is done, the consumer threads will continue until no work remains in the queue
  - Consumers also notify all other threads when one realizes the producer is done, in case any threads missed the producer's notification and then started waiting

- I made the decision to call the safeLog function outside of the critical sections for the safeGet and safeAdd methods. This presents the potential issue of events being logged at slightly incorrect times due to possible interleaving. However, I didn't want to have threads being in the queue add/get critical section and also in the logging critical section, because this could delay the logging for other threads.