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
  std::vector<std::shared_ptr<tcp::socket>> clients_;

void acceptConnection() {
    auto clientSocket = make_shared<tcp::socket>(acceptor_.get_executor());
    acceptor_.async_accept(*clientSocket, [this, clientSocket](const boost::system::error_code& ec) {
        if (!ec) {
            clients_.push_back(clientSocket);
            std::cout << "New client connected: " << clientSocket->remote_endpoint() << std::endl;
            startReading(clientSocket);
        }
        acceptConnection();
    });
}


void broadcastMessage(const std::string& message, std::shared_ptr<tcp::socket> sender) {
	for (auto& client : clients_)  {
		if (client != sender && client->is_open()) {
				try{
			boost::asio::write(*client, boost::asio::buffer(message + "\n"));
			}catch (const boost::system::system_error& e) {
        std::cerr << "Error sending message to client: " << e.what() << std::endl;
			}
		}
	}
}

void removeClient(std::shared_ptr<tcp::socket> clientSocket) {
		clients_.erase(remove(clients_.begin(), clients_.end(), clientSocket), clients_.end());
    std::cout << "Client disconected. " << clientSocket->remote_endpoint() << std::endl;

	}
	
void startReading(std::shared_ptr<tcp::socket> clientSocket) {
	auto self = clientSocket;
		boost::asio::async_read_until(*clientSocket, receiveBuffer_, '\n', [this, self](const boost::system::error_code ec, size_t length) {
			if (!ec) {
        std::istream is(&receiveBuffer_);
        std::string message;
				getline(is, message);

        std::cout << "Reveived from client " << self->remote_endpoint() << ": " << message << std::endl;

				broadcastMessage(message, self);
				
				receiveBuffer_.consume(length);
				startReading(self);
			} else {
				removeClient(self);
			}
		});
	}

public: 
	Chat(boost::asio::io_context& io_context, const tcp::endpoint& endpoint) : acceptor_(io_context, endpoint) {
    std::cout << "Server started. Waiting for clients to connect...\n";
		acceptConnection();
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