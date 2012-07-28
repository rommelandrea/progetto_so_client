CC = gcc
CFLAGS = -Wall -pedantic
LIBS = -lpthread
PROG = client

attuatore: client.c 
	$(CC) $(CFLAGS) -c client.c
	$(CC) client.o  -o $(PROG)

clean:
	rm -f *.o *~
	
cleanbin:
	rm -f $(PROG)