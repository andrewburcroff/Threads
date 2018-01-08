CC = gcc
CFlAGS = -g -Wall -pedantic -std=gnu99 -pthread

all: main

main: pthread.c
	${CC} ${CFlAGS} pthread.c
clean:
	rm -f pthread pthread.o
