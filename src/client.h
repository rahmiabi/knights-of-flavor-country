#pragma once
#include <boost/asio.hpp>
#include <boost/asio/read_until.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <chrono>
#include <deque>
using namespace std;
using boost::asio::ip::tcp;

class ChatClient {
    tcp::socket* socket_;
    boost::asio::io_context io_context_;
	bool ended = false;
	boost::asio::streambuf* receiveBuffer;

//Makes it to have 7 max msg on screen
	void addMessageToBuffer(const string& message) {
		messageBuffer_.push_front(message);

		if (messageBuffer_.size() > 7) {
			messageBuffer_.pop_back();
		}
	}

public:
    bool read = true;
	thread* receiveThread;
	deque<string> messageBuffer_;
    ChatClient(boost::asio::io_context& io_context, const tcp::resolver::results_type& endpoints)
        {
		socket_ = new tcp::socket(io_context);
        boost::asio::connect(*socket_, endpoints);
    }
	~ChatClient(){
		delete receiveThread;
		delete socket_;
	}
    void startChat() {
        try {
			receiveThread = new thread([this]() {
				while (read) {
					boost::asio::streambuf* receiveBuffer = new boost::asio::streambuf();
					try {
						boost::asio::async_read_until(*socket_, *receiveBuffer, '\n', [this, receiveBuffer](const boost::system::error_code ec, size_t length) {
							if (!ec) {
							istream is(receiveBuffer);
							string message;
							getline(is, message);
							addMessageToBuffer(message);
							}
						});
						delete receiveBuffer;
					} catch (boost::wrapexcept<boost::system::system_error> disconnectionError){
						delete receiveBuffer;
						ended = true;
						break;
					}
				}
				cout << "finished" << endl;
				return;
			});
			//Lets the thread to work on its own

            io_context_.poll();
        } catch (exception& e) {
            cerr << "Exception: " << e.what() << endl;
        }
    }
	void write(string& thing){
        boost::asio::write(*socket_, boost::asio::buffer(thing + "\n"));
	}
};

