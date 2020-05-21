PROG = codegen
CC = gcc
CFLAGS = 
LDFLAGS = 
OBJS = ll.o main.o codegen.o lexer.o semanal.o 

$(PROG) : $(OBJS)
	$(CC) $(LDFLAGS) -o $(PROG) $(OBJS)
ll.o : ll.h
	$(CC) $(CFLAGS) -c ll.c
main.o : 
	$(CC) $(CFLAGS) -c main.c
codegen.o : codegen.h
	$(CC) $(CFLAGS) -c codegen.c
lexer.o : lexer.h
	$(CC) $(CFLAGS) -c lexer.c
semanal.o : semanal.h
	$(CC) $(CFLAGS) -c semanal.c

clean:
	rm -f core $(OBJS)