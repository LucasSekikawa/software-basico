main: memory_alloc.o main.o
	gcc -Wall -g memory_alloc.o main.o -o main

memory_alloc.o: memory_alloc.s
	as -g memory_alloc.s -o memory_alloc.o

main.o: main.c
	gcc -Wall -g -c main.c -o main.o

clean:
	rm -f memory_alloc.o main.o main
