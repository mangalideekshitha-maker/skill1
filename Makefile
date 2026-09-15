CC = gcc
CFLAGS = -Wall -g

prog1: prog1.c
	$(CC) $(CFLAGS) -o prog1 prog1.c

prog2: prog2.c
	$(CC) $(CFLAGS) -o prog2 prog2.c
prog5: prog5.c
	gcc prog5.c -o prog5
clean:
	rm -f prog1 prog2


