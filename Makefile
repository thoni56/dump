#

CC	= gcc -ansi -pedantic -g 

dump: dump.o spa.o
	$(CC) -o dump dump.o spa.o

dump.o: dump.c spa.h

spa.o: spa.c spa.h
