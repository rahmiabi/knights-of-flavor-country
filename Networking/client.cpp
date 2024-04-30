#include <boost/asio/read_until.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
using namespace std;
using boost::asio::ip::tcp;

class ReadHandler {
    tcp::socket& socket_;
    boost::asio::streambuf& receiveBuffer_;
public:
    ReadHandler(tcp::socket& socket, boost::asio::streambuf& receiveBuffer)
        : socket_(socket), receiveBuffer_(receiveBuffer) {}

    // Operator () to act as the callback for async read operation
    void operator()(const boost::system::error_code& ec, size_t length) {
        if (!ec) {
			istream is(&receiveBuffer_);
			string message;
			getline(is, message);

			cout << "Received from server: " << message << endl;
            boost::asio::async_read_until(socket_, receiveBuffer_, '\n', *this);

        } else {
            cerr << "Error reading from server: " << ec.message() << endl;
            socket_.close();
        }
    }
};

int main()
{
	try
	{
		boost::asio::io_context io_context;
		tcp::socket socket(io_context);
		tcp::resolver resolver(io_context);

		auto endpoints = resolver.resolve("localhost", "6969"); //[1] == host, [2] == port

		boost::asio::connect(socket, endpoints);

		cout << "Connected to server. Type 'exit' to quit.\n";

		boost::asio::streambuf receiveBuffer;

		ReadHandler readHandler(socket, receiveBuffer);

		boost::asio::async_read_until(socket, receiveBuffer, '\n', readHandler);

		string userInput;
		while (getline(cin, userInput)) {
			if (userInput == "exit") {
				boost::asio::write(socket, boost::asio::buffer("CLIENT_DISCONNECTED\n"));
				break;
			} else {
				boost::asio::write(socket, boost::asio::buffer(userInput + "\n"));
			}

		}
		socket.close();
		cout << "Connection closed.\n";
	}
	catch (exception& e)
	{
		cout << "Exception: " << e.what() << endl;
	}
	return 0;
}
