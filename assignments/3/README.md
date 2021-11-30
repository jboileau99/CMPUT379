# CMPUT 379 Assignment 3

## Usage Instructions

Run "make" to generate "server" and "client" executables and PDF documents containing the man pages for each. Both executables can be run in the following form, respectively:
    "server port {<inputFile}"
    "client port ip-address {<inputFile}"

- Args:
  - port: Port is the number that the server reads from and the clients send to.
  - ip-address: IP address of the machine that is hosting the server
  - inputFile: Optional newline seperated file of T<N> or S<N> commands.
    

## Included files

### server.cpp / server.h

- Source code for a single threaded server that accepts up to 30 client connections simultaneously. 
- Will process transaction requests in the form T<N> and upon completion, respond to the client that sent the request.
- Maintains a log file of it's connections/actions.

### client.cpp / client.h

- Source code for a single threaded client that accepts input from stdin and sends transaction requests to a client, from which it awaits a response before doing further work. 
- Maintains a log file of it's connections/actions.

### helpers.cpp / helpers.h

- Timing and logging helper functions.

### tands.c

- Trans and sleep commands provided for assignment.

### server.1 / client.1

- Man page sources for server and client.

### Makefile

Targets:
- "make" or "make all"
- "make both": Runs both the "make server" and "make client" targets to produce "server" and "client" executables.
- "make server": Compiles all server required source and header files, links together, and produces "server" executable. 
- "make client": Compiles all client required source and header files, links together, and produces "client" executable.
- "make man": Generates PDF containing the man page text for both the client and server executables.
- "make clean": remove all intermediate .o files
- "make compress": used to generate .zip archive for assignment submission

## Assumptions

- No more than 30 clients will attempt to connect to the server at once
    
## Summary of Approach

My approach for the server was to have a single thread which maintains a master socket and then listens for connections from up to 30 difference clients. Upon receiving a connection from a new client, the server adds it's file descriptor to an array of file descriptors. 

The server then listens for an event on all of it's stored file descriptors and upon an event the server will process the activity. The server expects the activity to be one of:
    - Client connection closed
    - Transaction request
Any of other data will be ignored. If the client connection was closed, the server frees up the spot in it's file descriptor array that was previously occupied by that client.

If the data contained a transaction request (indicated by a T<N>) string, the server will service that request by calling Trans(), then respond back to the client indicating that the request was completed. The server will also also parse a -<ID> string from any transaction, which contains the ID of the client that sent the request. The server uses this ID to maintain a map containing stats on the number of requests per client. The server maintains a log file with it's actions and received requests. After not recievingany communication or new connections for 30 seconds, the server will log summary statistics then exit.
    
My approach for the client was to have a single thread that will attempt to connect to a server at an input IP and port. The client opens a socket with the server, then processes command input from either a file or stdin and expects commands in the form T<N> or S<N>. For each T command, it will send a request to the server and await a response. For each S command, the client will sleep. Upon reaching the end of the command input, the client closes the socket connection with the server, logs summary stats and exits.