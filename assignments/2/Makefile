# Declare variables in makefile

CC=g++
CFLAGS= -c -Wall -pthread
LFLAGS= -pthread

prodcon: prodcon.o helpers.o
	$(CC) $(LFLAGS) prodcon.o helpers.o -o prodcon

# Then each part compiles sperately to create object (.o) files like this:

prodcon.o: prodcon.cpp
	$(CC) $(CFLAGS) prodcon.cpp

helpers.o: helpers.cpp
	$(CC) $(CFLAGS) helpers.cpp
	
# Remove all intermediate files

clean:
	rm -rf *.o