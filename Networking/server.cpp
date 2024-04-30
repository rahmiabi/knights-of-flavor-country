#include <ctime>
#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <boost/asio.hpp>
#include <unistd.h>

using namespace std;
using boost::asio::ip::tcp;

class Chat{

	tcp::acceptor acceptor_;
	boost::asio::streambuf receiveBuffer_;
	vector<shared_ptr<tcp::socket>> clients_;

void acceptConnection() {
    auto clientSocket = make_shared<tcp::socket>(acceptor_.get_executor());
    acceptor_.async_accept(*clientSocket, [this, clientSocket](const boost::system::error_code& ec) {
        if (!ec) {
            cout << "New client connected: " << clientSocket->remote_endpoint() << endl;
            clients_.push_back(clientSocket);
            startReading(clientSocket);
        }
        acceptConnection();
    });
}


void broadcastMessage(const string& message, shared_ptr<tcp::socket> sender) {
	for (auto& client : clients_)  {
		if (client != sender && client->is_open()) {
				try{
			boost::asio::write(*client, boost::asio::buffer(message + "\n"));
			}catch (const boost::system::system_error& e) {
				cerr << "Error sending message to client: " << e.what() << endl;
			}
		}
	}
}

void removeClient(shared_ptr<tcp::socket> clientSocket) {
		clients_.erase(remove(clients_.begin(), clients_.end(), clientSocket), clients_.end());
		cout << "Client disconected. " << clientSocket->remote_endpoint() << endl;

	}
	
void startReading(shared_ptr<tcp::socket> clientSocket) {
	auto self = clientSocket;
		boost::asio::async_read_until(*clientSocket, receiveBuffer_, '\n', [this, self](boost::system::error_code ec, size_t length) {
			if (!ec) {
				istream is(&receiveBuffer_);
				string message;
				getline(is, message);

				cout << "Reveived from client: " << message << endl;

				broadcastMessage(message, self);
				
				receiveBuffer_.consume(length);
				startReading(self);

/*
				broadcastMessage(message, self);
				receiveBuffer_.consume(length);

				startReading(self);*/
	} else {
		removeClient(self);
	}
		});
	}

public: 
	Chat(boost::asio::io_context& io_context, const tcp::endpoint& endpoint) : acceptor_(io_context, endpoint) {
		cout << "Server started. Waiting for clients to connect...\n";
		acceptConnection();	;
	}
};
int main()
{
	try
	{
		//This is code to accept a connection as a server
		boost::asio::io_context io_context;
		tcp::endpoint endpoint(tcp::v4(), 6969); 

		Chat chat(io_context, endpoint);

		io_context.run();
		}
	catch (exception& e){
		cerr << "Exception: " << e.what() << endl;
	}
	return 0;
}
