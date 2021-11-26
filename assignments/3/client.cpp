/*
	Adapted from https://www.binarytides.com/server-client-example-c-sockets-linux/
*/
#include "tands.c"
#include "helpers.h"

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

int main(int argc , char *argv[]) {
	
    // Connections variables
    int connfd, message_size;
	struct sockaddr_in server;
    int server_size = sizeof(server);

    // Message vars
	char message[1024] , server_reply[1024];
    string message_str;
    message_size = sizeof(message);
    pair<char, int> cmd;

    // Stats and logging variables
    string ip;
    char hostname[HOST_NAME_MAX];
    int port;
    int transactions = 0;
    int server_transactions;
    gethostname(hostname, HOST_NAME_MAX);
    int pid = getpid();
    string id = string(hostname) + "." + to_string(pid);

    // Determine port and ip
    if (argc == 3) {
        // Set ID to default value of 0
        port = strtol(argv[1], nullptr, 0);
        ip = argv[2];

        // Check if port value is in the allowed range
        if (port < 5000 || port > 64000) {
            printf("ERROR: Port arguement must be in the range 5,000 to 64,000");
            exit(EXIT_FAILURE);
        }

    } else {
        printf("ERROR: Exactly 2 arguements expected");
        exit(EXIT_FAILURE);
    }
    
    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    server.sin_port = htons(port);

    // Create socket
    if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(-1);
    }

    // Connect to server
    if (connect(connfd, (struct sockaddr *)&server, server_size) < 0) {
        perror("connect failed. Error");
        exit(-1);
    }
    
    // Write log header
    writeLogHeaderClient(id, to_string(port), ip, id);

    // Process line of input
    for (string line; getline(cin, line);) {

        // Validate line
        if (line.size() >= 2 && (line[0] == 'T' || line[0] == 'S')) {

            // Split into pair of first and second character, converting second character to an integer
            pair<char, int> cmd (line[0], stoi(line.substr(1, string::npos)));

            // Producer adds work to queue
            if (cmd.first == 'T') {

                // Log command send and update count
                writeLogClient(id, "T", cmd.second);
                transactions++;

                // Append client details and copy to message for server
                line.append("-");
                line.append(id);
                strcpy(message, line.c_str());

                // Send command to server
                if (write(connfd, message, sizeof(message)) < 0) {
                    perror("write");
                    exit(-1);
                }

                // Wait for acknowledgement that transaction is done
                if (read(connfd, &message, sizeof(server_reply)) < 0) {
                    perror("read");
                    exit(-1);
                } else {

                    message_str.assign(message, message_size);

                    if (message_str.size() >= 2 && message_str[0] == 'D') {

                        // Extract transaction number
                        server_transactions = stoi(message_str.substr(1, string::npos));
                        
                        // Log server response
                        writeLogClient(id, "D", server_transactions);
                    }
                }

            } else if (cmd.first == 'S') {
                // Client sleeps
                writeLogClient(id, "S", cmd.second);
                Sleep(cmd.second);

            } else {
                printf("WARNING: Got invalid response from server.\n");
            }
        }
    }

    // Write transactions total and close connection
    writeLogClient(id, "E", transactions);
    close(connfd);
	return 0;
}