CC = g++
CFLAGS = -pthread -O3 -g

all: client server

client: client.cpp
	$(CC) $(CFLAGS) -o client client.cpp

server: server.cpp
	$(CC) $(CFLAGS) -o server server.cpp

clean:
	rm -f client server
