

gcd: gc.c gc.h
	gcc -g -DDEBUG -O -c -o gcd.o	gc.c
	
	
clean:
	rm -f gcd.o

