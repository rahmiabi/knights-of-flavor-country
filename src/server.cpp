#include <ctime>
#include <memory>
#include <iostream>
#include <vector>
#include <string>
#include <boost/asio.hpp>
#include <unistd.h>
#include <stdio.h>
#include <cmath>
#include <vector>
#include <random>
#include <chrono>
#include <memory>
#include <limits>
#include <thread>
#include <unordered_set>
#include <queue>
#include <string>
#include <fstream>
#include <sstream>

//#include <boost/asio/read_until.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
using namespace std;
using boost::asio::ip::tcp;

class Chat{
  int count = -1;
	tcp::acceptor acceptor_;
	boost::asio::streambuf receiveBuffer_;
	vector<shared_ptr<tcp::socket>> clients_;

void acceptConnection() {
    count++;
    auto clientSocket = make_shared<tcp::socket>(acceptor_.get_executor());
    acceptor_.async_accept(*clientSocket, [this, clientSocket](const boost::system::error_code& ec) {
        if (!ec) {
            clients_.push_back(clientSocket);
            cout << "New client connected: " << clientSocket->remote_endpoint() << endl;
            cout << count << endl;
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
		boost::asio::async_read_until(*clientSocket, receiveBuffer_, '\n', [this, self](const boost::system::error_code ec, size_t length) {
			if (!ec) {
				istream is(&receiveBuffer_);
				string message;
				getline(is, message);

				cout << "Reveived from client " << self->remote_endpoint() << ": " << message << endl;

				if (message == "DISCONNECT"){
					boost::asio::write(*self, boost::asio::buffer("\n"));
				} else broadcastMessage(message, self);
				
				receiveBuffer_.consume(length);
				startReading(self);
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