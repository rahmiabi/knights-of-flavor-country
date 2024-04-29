#include "Networking.h"

//Temp
ManageConnection::ManageConnection() : ioContext(), socket(ioContext), resolver(ioContext) {

}

ManageConnection::~ManageConnection() {
ShuttingDown();
}

bool ManageConnection::Initialize() {
	try{ 
	return true;
	}
	catch (std::exception& e) {
		std::cerr << "Unable to initialize: " << e.what() << std::endl;
		return false;
	}
}

void ManageConnection::ShuttingDown() {
	if (socket.is_open()) {
		socket.close();
	}
}
bool ManageConnection::Connecting(const std::string& ip, const std::string& port) {
	try {
		boost::asio::ip::tcp::resolver::results_type endpoint  = resolver.resolve(ip, port);	
		boost::asio::connect(socket, endpoint);
		return true;
	}
	catch (std::exception& e) {
		std::cerr << "Error when trying to connect: " << e.what() << std::endl;
		return false;
	}
}
void ManageConnection::Disconnecting() {
	if (socket.is_open()) {
		boost::system::error_code ec;
		socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
		if (ec) {
			std::cerr << "Error shutting down socket: " << ec.message() << std::endl;
		}
		socket.close();
	}
}


bool ManageConnection::Send(const std::string & data) {
	try{
		boost::asio::write(socket, boost::asio::buffer(data));
		return true;
	}
	catch (const std::exception& e) {
		std::cerr << "Error when sending data: " << e.what() << std::endl;
		return false;
	}
}

std::string ManageConnection::Recieve(){
	try{
		boost::asio::streambuf recieveBuffer;
		boost::asio::read_until(socket, recieveBuffer, "\n");
		std::string data_recieved = boost::asio::buffer_cast<const char*>(recieveBuffer.data());
		return data_recieved;
	}
	catch(const std::exception& e) {
		std::cerr << "Error during receiving data: " << e.what() << std::endl;
		return "";
	}

}
