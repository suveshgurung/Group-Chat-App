CC = gcc
CFLAGS = -Wall

all: group_chat_app

group_chat_app: server.o client.o utilities.o
	$(CC) $(CFLAGS) -o server server.o utilities.o && $(CC) $(CFLAGS) -o client client.o utilities.o

server.o: server.c utilities.h
	$(CC) $(CFLAGS) -c server.c

client.o: client.c utilities.h
	$(CC) $(CFLAGS) -c client.c

utilities.o: utilities.c utilities.h
	$(CC) $(CFLAGS) -c utilities.c

clean:
	rm -rf server client *.o
