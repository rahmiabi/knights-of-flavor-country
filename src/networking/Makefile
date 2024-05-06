CC = g++
CFLAGS = -pthread -O3 -g
LIBS = -lncurses
# -L/mingw64/lib/libboost_system -lws2_32 -lwsock32
all: client server

client: client.cpp
	$(CC) $(CFLAGS) -o client client.cpp $(LIBS)

server: server.cpp
	$(CC) $(CFLAGS) -o server server.cpp

clean:
	rm -f client server
