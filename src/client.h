#pragma once
#include <boost/asio.hpp>
#include <boost/asio/read_until.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <deque>
using namespace std;
using boost::asio::ip::tcp;

class ChatClient {
    tcp::socket socket_;
    boost::asio::io_context io_context_;
    bool inChatMode_ = false;

//Makes it to have 7 max msg on screen
	void addMessageToBuffer(const string& message) {
		messageBuffer_.push_front(message);

		if (messageBuffer_.size() > 7) {
			messageBuffer_.pop_back();
		}
	}

public:
	deque<string> messageBuffer_;
    ChatClient(boost::asio::io_context& io_context, const tcp::resolver::results_type& endpoints)
        : socket_(io_context) {
        boost::asio::connect(socket_, endpoints);
    }
    void startChat() {
        try {
			thread receiveThread([this]() {
				while (true) {
					boost::asio::streambuf receiveBuffer;
					boost::asio::read_until(socket_, receiveBuffer, '\n');
					istream is(&receiveBuffer);
					string message;
					getline(is, message);
		//Original, not part of testing chat
				//	mvwprintw(outputWindow, 2, 0, message.c_str());
				//	wrefresh(outputWindow);

					addMessageToBuffer(message);

				}
			});
			//Lets the thread to work on its own
			receiveThread.detach();

                // Process asynchronous operations
                io_context_.poll();
        } catch (exception& e) {
            cerr << "Exception: " << e.what() << endl;
        }
    }
	void write(string& thing){
        boost::asio::write(socket_, boost::asio::buffer(thing + "\n"));
	}
};

