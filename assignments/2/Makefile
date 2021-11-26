CC=g++
CFLAGS= -O -Wall -pthread
OBJECTS = prodcon.o helpers.o

prodcon: prodcon.o helpers.o
	$(CC) $(CFLAGS) $(OBJECTS) -o prodcon

# Create object (.o) files for each:

prodcon.o: prodcon.cpp prodcon.h tands.c
	$(CC) $(CFLAGS) -c prodcon.cpp -o prodcon.o

helpers.o: helpers.cpp helpers.h
	$(CC) $(CFLAGS) -c helpers.cpp -o helpers.o
	
# Remove all intermediate files

clean:
	rm -rf *.o

# Compress for submission

compress:
	zip assignment2.zip Makefile README.md prodcon.h prodcon.cpp helpers.h helpers.cpp tands.c