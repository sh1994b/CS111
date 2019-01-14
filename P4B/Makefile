#NAME: Shabnam Bahmanyar
#EMAIL: sh1994b@g.ucla.edu
#ID: 104950171

C = gcc
CFLAGS = -g -Wall -Wextra -lmraa -lm
.SILENT:

default: lab4b
lab4b: lab4b.c
	$(CC) $(CFLAGS) -o lab4b lab4b.c

check: default

clean:
	-rm -f lab4b
	-rm -f *.tar.gz

dist: default
	tar -zcf lab4b-104950171.tar.gz lab4b.c Makefile README
