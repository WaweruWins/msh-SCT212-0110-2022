CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11 -D_DEFAULT_SOURCE

msh: msh.c
	$(CC) $(CFLAGS) -o msh msh.c

clean: 
	rm -f msh