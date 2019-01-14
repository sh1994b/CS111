
C = gcc
CFLAGS = -g -Wall -Wextra
.SILENT:

default: lab1a
lab1a: lab1a.o
	$(CC) $(CFLAGS) -o lab1a lab1a.o
lab1a.o: lab1a.c
	$(CC) $(CFLAGS) -c  lab1a.c

clean: dist
	-rm -f *.o
	-rm -f lab1a
	-rm -f *.txt
	-rm -f *.tar.gz
	$(dist)
dist:
	tar -zcf lab1a-104950171.tar.gz lab1a.c README Makefile


