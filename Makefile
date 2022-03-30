CC=gcc

myshell: myshell.c
	$(CC) -o $@ $<