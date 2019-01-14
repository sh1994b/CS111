
C = gcc
CFLAGS = -g -Wall -Wextra -lmcrypt
.SILENT:

default: lab1b-client lab1b-server
lab1b-client: lab1b-client.o
	$(CC) $(CFLAGS) -o lab1b-client lab1b-client.o
lab1b-client.o: lab1b-client.c
	$(CC) $(CFLAGS) -c  lab1b-client.c
lab1b-server: lab1b-server.o
	$(CC) $(CFLAGS) -o lab1b-server lab1b-server.o
lab1b-server.o: lab1b-server.c
	$(CC) $(CFLAGS) -c  lab1b-server.c



clean: dist
	-rm -f *.o
	-rm -f lab1b-client
	-rm -f lab1b-server
	-rm -f *.txt
	-rm -f *.tar.gz
	$(dist)
dist:
	tar -zcf lab1b-104950171.tar.gz lab1b-client.c lab1b-server.c my.key README Makefile


