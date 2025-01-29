OBJ = fgreedy.o
CC = gcc
LFLAGS = -g
VALGRIND=valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck
CFLAGS = -O3

SRC = fgreedy.c tools.c pp.c 
ALLSRC = ${SRC} fgreedy.h Makefile *.cnf pl3d.py

all: fgreedy


fgreedy6:
	gcc -O3 -DNEIGHBOURHOODSIZE=6 -o fgreedy6 fgreedy.c tools.c pp.c 

fgreedy18:
	gcc -O3 -DNEIGHBOURHOODSIZE=18 -o fgreedy18 fgreedy.c tools.c pp.c 


pp.o: pp.c fgreedy.h
tools.o: tools.c fgreedy.h pp.c
fgreedy.o: fgreedy.c fgreedy.h tools.o pp.o
fgreedy: fgreedy.o tools.o 
	gcc -O3 -o fgreedy fgreedy.o tools.o pp.o 

profiler72: 
	gcc -Wall -pg -O3 -o fgreedy ${SRC}
	fgreedy  k72_z31_xyz_LOC75.tsv -x paramsnotime.cnf -vtTxpG1 -k72 -n1
	gprof fgreedy

profiler14: 
	gcc -Wall -pg -O3 -o fgreedy ${SRC}
	fgreedy  k72_z31_xyz_LOC75.tsv -x paramsnotime.cnf -vtTxpG1 -k14 -n5
	gprof fgreedy


profilerlines: 
	gcc -Wall -pg -fprofile-arcs -ftest-coverage -o fgreedy ${SRC}
	fgreedy  k72_z31_xyz_LOC75.tsv -x paramsnotime.cnf -vtTxpG1 -k72 -n1
	gcov fgreedy

large: 
	gcc -DLARGE -O3 -o fgreedy ${SRC}

ts: clean
	gcc -DTIMESTATS -O3 -o fgreedyts ${SRC}	


fast: 
	gcc -Ofast -o fgreedy ${SRC}

clean: 
	rm -f fgreedy *.o

tgz: 
	tar czvf fgreedy.tgz ${ALLSRC}

gdb: 
	gcc -g -o fgreedy ${SRC}

