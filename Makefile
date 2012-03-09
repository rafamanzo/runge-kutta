main: main.c input.h rk.h c_version input
	gcc main.c input.o rk.o -Wall -ansi -o rk
	
input: input.h rk.h
	gcc -c -Wall -ansi input.c

c_version: c/rk.c rk.h
	gcc -c -Wall -ansi c/rk.c
	
clean:
	rm -f *~ *.o rk
