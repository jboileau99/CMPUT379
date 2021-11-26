CC=g++
CFLAGS= -O -Wall
CLIENT_OBJECTS = client.o helpers.o
SERVER_OBJECTS = server.o helpers.o

both: client server

client: client.o helpers.o
	$(CC) $(CFLAGS) $(CLIENT_OBJECTS) -o client

server: server.o helpers.o
	$(CC) $(CFLAGS) $(SERVER_OBJECTS) -o server

# Create object (.o) files for each:

client.o: client.cpp client.h
	$(CC) $(CFLAGS) -c client.cpp -o client.o

server.o: server.cpp server.h tands.c
	$(CC) $(CFLAGS) -c server.cpp -o server.o

helpers.o: helpers.cpp helpers.h
	$(CC) $(CFLAGS) -c helpers.cpp -o helpers.o
	
# Remove all intermediate files

clean:
	rm -rf *.o

# Compress for submission

compress:
	zip assignment2.zip Makefile README.md client.h client.cpp server.h server.cpp helpers.h helpers.cpp tands.c