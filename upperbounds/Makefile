OBJ = fgreedy.o
CC = gcc
LFLAGS = -g
VALGRIND=valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck
CFLAGS = -O3

SRC = fgreedy.c tools.c pp.c 
ALLSRC = ${SRC} fgreedy.h Makefile *.cnf pl3d.py

#all: gdb 
all: fgreedy
#all: fgreedy2

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
	#gcc -g -O3 -o fgreedy ${SRC}


v1: gdb
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck fgreedy Xu2108/data/k14_rft_xyz_Cerebellum52.tsv -x batch6x.cnf -k14 -v123 -n 2 -N 6 -T-1  




# g90: fgreedy.c
# 	gcc -g -o fgreedy fgreedy.c
# 	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck fgreedy -a5 -A1 -s10 -k90 -r1000 k90.csv

# gj: fgreedy.c
# 	gcc -g -O3 -o fgreedy fgreedy.c
# 	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --tool=memcheck fgreedy -vu -a5 -A1 -s2 -k10 -E5 -e5 -r200 jellevoxels.tsv

# gj2: fgreedy.c
# 	rm -f core
# 	gcc -g -O3 -o fgreedy fgreedy.c
# 	fgreedy -a5 -A1 -s2 -k10 -E5 -e5 -r200 jellevoxels.txt

# g448: fgreedy.c
# 	gcc -g -o fgreedy fgreedy.c
# 	${VALGRIND} fgreedy -vt -a15 -A1 -s5 -E3 -e3 -k448 -r10 k448.csv 

loc: gdb
	${VALGRIND} fgreedy  k72_z31_xyz_LOC75.tsv -x params3.cnf -vP -k72

sq: gdb
	${VALGRIND} fgreedy  square.tsv -x params3.cnf -vP123 -k2 -N0.1

sq3: gdb
	${VALGRIND} fgreedy  squares3x.tsv -x params3.cnf -vP123 -k2 -N0.1

sq4: gdb
	${VALGRIND} fgreedy Xu2108/data/k32_z31_xyz_SMG75.tsv -n 1000000 -N 60 -T-1 -k32 -vtTxpf -x batch6.cnf -f.r1

sq5: gdb
	${VALGRIND} fgreedy -x batch6.cnf -voGPpsq -k5 -N1 :4x5


sq6: gdb
	${VALGRIND} fgreedy -x batch6.cnf -voGPpsq -k200 -N1 :4x5

# sq: d
# 	gcc -O3 -g -o fgreedy fgreedy.c	
# 	${VALGRIND} fgreedy -vtTx -a1 -A1 -s1 -r1 -k10 -E1 -e1 -q0.4 jellevoxels/24.tsv -R  2>vres.txt 

