# Makefile for GC, Dave's C garbage-collection utility. Dave's (me) first name is actually Lee.
# Nobody cared about Marvin, nobody cares about me. At least I'm not paranoid about it.

.PHONY: clean

all: gc gcd gctest gctestd


gc: gc.c gc.h
	gcc gc.c -O3 -c -o gc.o

gcd: gc.c gc.h
	gcc gc.c -g -DDEBUG -O0 -c -o gcd.o


gctest: gc
	gcc gctest.c gc.o -mconsole -O2 -o gctest.exe

gctestd: gcd
	gcc gctest.c gcd.o -g -DDEBUG -mconsole -O0 -o gctestd.exe


clean:
	rm -f gcd.o
	rm -f gc.o
	rm -f gctest.exe
	
# EOF
