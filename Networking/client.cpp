#include <boost/asio/read_until.hpp>
#include <cstdlib>
#include <iostream>
#include <string>
#include <boost/asio.hpp>
#include <ncurses.h>
#include <curses.h>
#include <deque>
using namespace std;
using boost::asio::ip::tcp;
/*
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
            async_read_until(socket_, receiveBuffer_, '\n', *this);

        } else {
            cerr << "Error reading from server: " << ec.message() << endl;
            socket_.close();
        }
    }
};
*/
class ChatClient {
    tcp::socket socket_;
    boost::asio::io_context io_context_;
    bool inChatMode_ = false;
    string userInput_;
	deque<string> messageBuffer_;

	void displayMessage(WINDOW* outputWindow) {
		if (inChatMode_) {
			mvwprintw(outputWindow, 0, 0, "Chat Mode. Press 'esc' to exit\n");
		}else {
			mvwprintw(outputWindow, 0, 0, "Connected to server. Type 'exit' to quit. Press '/' to start caht.\n");
		}
		wrefresh(outputWindow);
	}
//Makes it to have 7 max msg on screen
	void addMessageToBuffer(const string& message) {
		messageBuffer_.push_front(message);

		if (messageBuffer_.size() > 7) {
			messageBuffer_.pop_back();
		}
	}

	void displayMessages(WINDOW* outputWindow) {

		werase(outputWindow);
		int numMessages = min(static_cast<int>(messageBuffer_.size()), 10);
		int starty = max(1, LINES - numMessages - 1);

		for (int i = 0; i <  numMessages; i++) {
			const string& msg = messageBuffer_[i];
			mvwprintw(outputWindow, starty + i, 0, msg.c_str());
		}

	if (!userInput_.empty()) {
			string userMessage = "You: " + userInput_;
		mvwprintw(outputWindow, LINES - 1, 0, userMessage.c_str());
		}
		wrefresh(outputWindow);
	}
public:
    ChatClient(boost::asio::io_context& io_context, const tcp::resolver::results_type& endpoints)
        : socket_(io_context) {
        boost::asio::connect(socket_, endpoints);
    }
    void startChat(WINDOW* outputWindow) {
        try {
			thread receiveThread([this, outputWindow]() {
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

					displayMessages(outputWindow);
					displayMessage(outputWindow);
				}
			});
			//Lets the thread to work on its own
			receiveThread.detach();

            while (true) {
                int ch = wgetch(outputWindow);

                if (ch == '/') {
                    inChatMode_ = !inChatMode_;
                    clear();
					displayMessages(outputWindow);
					displayMessage(outputWindow);
                } else if (ch == 27) { // 'esc' key
                    inChatMode_ = false;
                    clear();
					displayMessage(outputWindow);
                }

                if (inChatMode_ && ch != ERR) {
                    if (ch == '\n') {
                        // Send user input to the server
                        boost::asio::write(socket_, boost::asio::buffer(userInput_ + "\n"));

                        if (userInput_ == "exit") {
                            break;
                        }

						//Adds your message to the chat
						addMessageToBuffer("You: " + userInput_);

                        userInput_.clear();
                    } else if (ch == KEY_BACKSPACE || ch == 127) {
                        // Handle backspace
                        if (!userInput_.empty()) {
                            userInput_.pop_back();
                            clear();
							displayMessages(outputWindow);
							displayMessage(outputWindow);
                            mvwprintw(outputWindow, 1, 0, userInput_.c_str());
                        }
                    } else {
                        userInput_ += static_cast<char>(ch);
                        clear();
						displayMessages(outputWindow);
						displayMessage(outputWindow);
                        mvwprintw(outputWindow, 1, 0, userInput_.c_str());
                    }
                }

                // Process asynchronous operations
                io_context_.poll();
            }
        } catch (exception& e) {
            cerr << "Exception: " << e.what() << endl;
        }
    }
};

int main() {
    try {
        // Initialize ncurses
        WINDOW* outputWindow = initscr();
        if (outputWindow == nullptr) {
            cerr << "Error initializing ncurses\n";
            return 1;
        }
        cbreak();
        noecho();
        keypad(outputWindow, TRUE);

        // Create a new window for output
        mvwprintw(outputWindow, 0, 0, "Connected to server. Type 'exit' to quit. Type '/' to start chat.\n");
        wrefresh(outputWindow);

        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("localhost", "6969");

        ChatClient client(io_context, endpoints);
        client.startChat(outputWindow);

        // Cleanup ncurses
        delwin(outputWindow);
        endwin(); // Restore terminal to normal state

        cout << "Connection closed.\n";
    } catch (exception& e) {
        cerr << "Exception: " << e.what() << endl;
    }

    return 0;
}
