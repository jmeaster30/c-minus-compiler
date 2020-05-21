PROG = lex
CC = gcc
CFLAGS = 
LDFLAGS = 
OBJS = main.o 

$(PROG) : $(OBJS)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJS)
main.o : 
	$(CC) $(CFLAGS) -c main.c

clean:
	rm -f core $(OBJS)