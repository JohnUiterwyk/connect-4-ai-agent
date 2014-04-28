
CC=gcc
CFLAGS=-Wall -pedantic -ansi -O2 -g 
LINKFLAGS=-lm

c4: game.o c4.o c4_functions.o get_agents.o 
	$(CC) $(CFLAGS) $(LINKFLAGS) -o c4 game.o c4.o c4_functions.o get_agents.o agent_*.o

game.o: game.c
	$(CC) $(CFLAGS) -c game.c -o $@

c4.o: c4.c
	$(CC) $(CFLAGS) -c c4.c -o $@

c4_functions.o: c4_functions.c
	$(CC) $(CFLAGS) -c c4_functions.c -o $@

get_agents.o: *.c createAgentList.sh
	sh createAgentList.sh "$(CC) $(CFLAGS)"        
	$(CC) $(CFLAGS) -c get_agents.c -o $@

   
           
clean:
	rm -f *.o get_agents.c c4

