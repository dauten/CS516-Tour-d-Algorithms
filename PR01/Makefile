### Basic Makefile for OpenMP (fromhttps://www.linuxtoday.com/blog/openmp-in-30-minutes.html) 

CC = gcc
CFLAGS = -g -O0 -fopenmp
LD = gcc
LDFLAGS = -g -fopenmp

PROGRAM = openmp

all:	${PROGRAM}.exe

${PROGRAM}.exe: ${PROGRAM}.o
	${LD} ${LDFLAGS} $< -o ${PROGRAM}.exe

${PROGRAM}.o: ${PROGRAM}.c
	${CC} ${CFLAGS} -c $< -o ${PROGRAM}.o

clean:
	rm -f ${PROGRAM}.o ${PROGRAM}.exe
