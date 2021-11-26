/*
	Adapted from https://www.binarytides.com/server-client-example-c-sockets-linux/
*/
#include "tands.c"
#include "helpers.h"

#include <stdio.h>
#include <string>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
#include <map>
#include <chrono>
#include <boost/algorithm/string.hpp>

using namespace std;

typedef map<string, int> MapType;  

int main(int argc , char **argv) {

	// Connection variables
	int listenfd, connfd;
	struct sockaddr_in server, client;
    int client_length = sizeof(client);
	
	// Message variables
	char message[1024];
	string message_str;
	int message_size = sizeof(message);
	pair<char, int> cmd;
	string client_id;

	// Server response variables
	string response;
	int transactions = 0;

	// Stats variables
	map<string, int> stats;
	char hostname[HOST_NAME_MAX];
	int port;
	int result = gethostname(hostname, HOST_NAME_MAX);
	int pid = getpid();
	auto firstStart = chrono::high_resolution_clock::now();
	string id = string(hostname) + "." + to_string(pid);
	
    // Determine port
    if (argc == 2) {
        // Set ID to default value of 0
        port = strtol(argv[1], nullptr, 0);
		printf("Using port %d\n", port);
    } else {
        perror("Exactly 1 arguement expected");
		exit(1);
    }

	// Create socket
    if ((listenfd=socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(-1);
    }
	fcntl(listenfd, F_SETFL, O_NONBLOCK);
	
	// Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);
	
	// Bind
	if (bind(listenfd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		perror("bind");
		exit(-1);
	} else {
		printf("Bind done\n");
	}
	
	// Listen
	if (listen(listenfd, 10) < 0) {
        perror("listen");
        exit(-1);
    } else {
		printf("Waiting for incoming connections...\n");
	}

	// Write log header
	writeLogHeaderServer(id, to_string(port));

	// Timing variable
	auto start = chrono::high_resolution_clock::now();
	
	// Repeatedly check for client connections and service them
	while (getElapsed(start) < 10) {

		// Accept connection from an incoming client
		if ((connfd=accept(listenfd, (struct sockaddr *)&client, (socklen_t*)&client_length)) < 0) {

			// Indicates no connections are present to be accept
			if (errno != 11) {
				perror("accept");
				exit(-1);
			}
		} else {

			// Restart 30 second timeout
			start = chrono::high_resolution_clock::now();
			printf("Connection accepted\n");

			// Read message from this client
			if (read(connfd, message, message_size) != message_size) {
				perror("read");
				close(connfd);
				exit(-1);
			} else {

				// Convert char array to String
				message_str.assign(message, message_size);
				
				// Service request from client
				if (message_str.size() >= 2 && message_str[0] == 'T') {

					// Extract command and client ID from message and call Trans
					cmd = pair<char, int>(message_str[0], stoi(message_str.substr(1, message_str.find("-"))));
					client_id = message_str.substr(message_str.find("-") + 1, string::npos);
					boost::trim(client_id);
					printf("Got: C: %c, l: %d from %s\n", cmd.first, cmd.second, client_id.c_str());

					// Increment stats and log
					transactions++;

					MapType::iterator lb = stats.lower_bound(client_id);
					if(lb != stats.end() && !(stats.key_comp()(client_id, lb->first))) {
						// key already exists
						// update lb->second if you care to
						printf("Key exists");
						lb->second++;
					} else {
						// the key does not exist in the map
						// add it to the map
						printf("Key not exists");
						stats.insert(lb, MapType::value_type(client_id, 1));
					}
					// if (stats.find(client_id) == stats.end() ) {
					// 	stats[client_id] = 1;
					// } else {
					// 	stats[client_id]++;
					// }
					writeLogServer(id, "T", transactions, cmd.second, client_id);

					// Service request
					Trans(cmd.second);
					
					// Log done
					writeLogServer(id, "D", transactions, cmd.second, client_id);

					// Build response
					response = "D" + to_string(transactions);
					strcpy(message, response.c_str());

					// Send response
					if (write(connfd, &message, message_size) != message_size) {
						perror("write");
						exit(-1);
					}
				}

				// Close connection with client
				close(connfd);
			}
		}
	}

	// Log summary stats - something here isn't working... try converting to char*?
	printf("stats size: %d", stats.size());
	auto diff = start - firstStart;
    float seconds = chrono::duration <float> (diff).count();
	writeLogStatsServer(id, stats, seconds);

	// Close connection before finishing
	close(listenfd);
	return 0;
}