//Gonna try and do socket stuff

#ifndef NETWORKING_H
#define NETWORKING_H


#include <boost/asio/ip/tcp.hpp>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <boost/asio.hpp>

class ManageConnection {
	//Does I/O
	boost::asio::io_context ioContext;
	//TCP socket communication
	boost::asio::ip::tcp::socket socket;
	//"Resolves hostnames and server names to IP endpoints"
	boost::asio::ip::tcp::resolver resolver;

public:
	//Constructor
	ManageConnection();
	//Destructor
	~ManageConnection();

	//Initializes the netorking
	bool Initialize();
	//Shuts down networking, self explanatory
	void ShuttingDown();

	//Creates a connection at specified IP and port 
	bool Connecting(const std::string& ip, unsigned short port);
	//Destroys connection
	void Disconnecting();

	//Should send data to other devices
	bool Send(const std::string & data);
	//Recieves data
	std::string Recieve();
};

#endif // NETWORKING_H
