/*
	Adapted from https://www.geeksforgeeks.org/socket-programming-in-cc-handling-multiple-clients-on-server-without-multi-threading/
*/
#include "tands.c"
#include "helpers.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <limits.h>
#include <sys/time.h>
#include <string>
#include <unordered_map>

using namespace std;

int main(int argc , char *argv[]) {

    // Connection variables
    int master_socket, addrlen, new_socket, client_socket[30], activity, i, sd, max_sd, work_time;
    int opt = 1;  
    int max_clients = 30;
    struct sockaddr_in address;

    // Message varialbes
    char buffer[1024];
    int valread;
    string message_str, client_id, response;
         
    // Set of socket descriptors 
    fd_set connfds;

    // Timeout for the select function
    struct timeval timeout;

    // Stats variables
    unordered_map<string, int> stats;
    int transactions = 0;

    // Identification variables
    char hostname[HOST_NAME_MAX];
	int pid = getpid();
	int port;
	gethostname(hostname, HOST_NAME_MAX);
	string id = string(hostname) + "." + to_string(pid);

    // Timing variable
	auto firstStart = chrono::high_resolution_clock::now();

    // Determine port
    if (argc == 2) {
        // Set ID to default value of 0
        port = strtol(argv[1], nullptr, 0);

        // Check if port value is in the allowed range
        if (port < 5000 || port > 64000) {
            printf("ERROR: Port arguement must be in the range 5,000 to 64,000\n");
            exit(EXIT_FAILURE);
        }
        
    } else {
        printf("ERROR: Exactly 1 arguement expected\n");
		exit(EXIT_FAILURE);
    }
     
    // Initialize all client sockets to 0 so not checked 
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }
         
    // Create master socket
    if ((master_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }  
     
    // Set master socket to allow multiple connections 
    if (setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 ) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }  
     
    // Type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
         
    // Bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }  
         
    // Try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }  
         
    // Accept the incoming connection
    addrlen = sizeof(address);

    // Write log header
	writeLogHeaderServer(id, to_string(port));

    // Timing variable
	auto start = chrono::high_resolution_clock::now();

    while(true) {
        
        // Clear the socket set 
        FD_ZERO(&connfds);
     
        // Add master socket to set 
        FD_SET(master_socket, &connfds);
        max_sd = master_socket;
             
        // Add child sockets to set 
        for (i = 0; i < max_clients; i++) {
            
            // Socket descriptor 
            sd = client_socket[i];
                 
            // Add to file descriptor list if socket descriptor is valid
            if(sd > 0) {
                FD_SET( sd, &connfds);
            }
                 
            // Keep track of the max socket descriptor number for the select function
            if (sd > max_sd) {
                max_sd = sd;
            }
        }

        // Reset timeout
        timeout.tv_sec = 30;
        timeout.tv_usec = 0;
        start = chrono::high_resolution_clock::now();
     
        // Wait for one of the sockets to have data
        activity = select(max_sd + 1, &connfds, NULL, NULL, &timeout);  
       
        if ((activity < 0) && (errno!=EINTR)) {  
            printf("select error");  
        } else if (activity == 0) {
            // Timeout expired
            break;
        }
             
        // If event is on master socket, it's a new connection
        if (FD_ISSET(master_socket, &connfds)) {
            
            if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }
                                  
            // Add new socket to array of sockets 
            for (i = 0; i < max_clients; i++) {
                
                // If position is empty
                if( client_socket[i] == 0 ) {
                    client_socket[i] = new_socket;
                    break;  
                }  
            }  
        }
             
        // Otherwise check which socket the event was for
        for (i = 0; i < max_clients; i++) {  
            
            sd = client_socket[i];  
                 
            if (FD_ISSET(sd, &connfds)) {  
                
                // Read data and check if it contains the termination message
                if ((valread = read(sd, buffer, 1024)) == 0) {  
                    
                    // Get details of disconnected FD
                    getpeername(sd ,(struct sockaddr*)&address ,(socklen_t*)&addrlen);
                         
                    // Close the socket and mark as 0 so it will be reused 
                    close(sd);
                    client_socket[i] = 0;

                } else {  
                    
                    // Get client ID and message strings
                    buffer[valread] = '\0';
                    message_str.assign(buffer);
                    client_id = message_str.substr(message_str.find("-") + 1, string::npos);
        
                    if (buffer[0] == 'T') {

                        // Get work time and incremement transaction count
                        work_time = stoi(message_str.substr(1, message_str.find("-")));
                        transactions++;

                        // Call Trans, logging before and after
                        writeLogServer(id, "T", transactions, work_time, client_id);
                        Trans(work_time);
                        writeLogServer(id, "D", transactions, work_time, client_id);

                        // Update stats
                        if (stats.find(client_id) == stats.end()) {
                            // Not found
                            stats.insert({client_id, 1});
                        } else {
                            // Found
                            stats[client_id]++;
                        }

                        // Send response
                        response = "D" + to_string(transactions);
                        strcpy(buffer, response.c_str());
                        send(sd , buffer, strlen(buffer), 0);
                    }
                }
            }
        }
    }

    // Log summary stats
	auto diff = start - firstStart;
    auto seconds = chrono::duration <float> (diff).count();
	writeLogStatsServer(id, stats, seconds, transactions);

	// Close connection before finishing
	close(master_socket);
    return 0;
}