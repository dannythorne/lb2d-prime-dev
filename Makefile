################################################################################
#
# Copyright (C), 2005, Michael Sukop and Danny Thorne
#
# Makefile
#

EXE=lb2d_prime

all:
	gcc -DOSTYPE=default -o ${EXE} ./src/lb2d_prime.c -lm

ostype:
	gcc -DOSTYPE=${OSTYPE} -o ${EXE} ./src/lb2d_prime.c -lm

par:
	mpicc -DPARALLEL -DOSTYPE=${OSTYPE} -o ${EXE} ./src/lb2d_prime.c -L/sw/lib -I/sw/include/ -lmpich -lm

swap:
	gcc -DSWAP_BYTE_ORDER -o ${EXE} ./src/lb2d_prime.c -lm

swap_byte_order:
	gcc -DSWAP_BYTE_ORDER -o ${EXE} ./src/lb2d_prime.c -lm

par_swap:
	gcc -DPARALLEL -DSWAP_BYTE_ORDER -DOSTYPE=${OSTYPE} -o ${EXE} ./src/lb2d_prime.c -L/sw/lib -I/sw/include/ -lmpi -llam -lm

warn:
	gcc -pedantic -Wall -Wunused-variable -o ${EXE} ./src/lb2d_prime.c -lm

nocygwin:
	gcc -mno-cygwin -o ${EXE} ./src/lb2d_prime.c -lm

slice: new_slice.c lbio.c
	gcc -o slice ./src/new_slice.c -lm

spy: ./src/spy.c
	gcc -o spy ./src/spy.c -lm

ascii: ./src/ascii_view_density.c
	gcc -o ascii_view_density ./src/ascii_view_density.c -lm

sweep:
	/bin/rm -f ./out/*.dat
	/bin/rm -f ./out/rho*.txt
	/bin/rm -f ./out/u*.txt
	/bin/rm -f ./out/force*.txt
	/bin/rm -f ./out/f*.txt
	/bin/rm -f ./out/*.txt
	/bin/rm -f ./out/rho*.bmp
	/bin/rm -f ./out/ueq*.bmp
	/bin/rm -f ./out/u*.bmp
	/bin/rm -f ./out/vor*.bmp
	/bin/rm -f ./out/force*.bmp
	/bin/rm -f ./out/sforce*.bmp
	/bin/rm -f *~ .*.sw*
	/bin/rm -f ./out/*~ ./out/.*.sw*
	/bin/rm -f ./in/*~ ./in/.*.sw*
	/bin/rm -f ./src/*~ ./src/.*.sw*
	/bin/rm -f ./out/slice*.m
	/bin/rm -f ./out/new_slice*.m

clean:
	/bin/rm -f ${EXE}.exe slice.exe ${EXE}.exe* *.stackdump dump*.*

xclean:
	/bin/rm -f ${EXE} slice core dump*.*
