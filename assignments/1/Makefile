# Declare variables in makefile

CC=g++
CFLAGS= -c -Wall

shell: shell.o cmds.o helpers.o
	$(CC) shell.o cmds.o helpers.o -o shell
	./shell

# Then each part compiles sperately to create object (.o) files like this:

shell.o: shell.cpp
	$(CC) $(CFLAGS) -c shell.cpp

cmds.o: cmds.cpp
	$(CC) $(CFLAGS) -c cmds.cpp

helpers.o: helpers.cpp
	$(CC) $(CFLAGS) -c helpers.cpp
	

# Remove all intermediate files

clean:
	rm -rf *.o