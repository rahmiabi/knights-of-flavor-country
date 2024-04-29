#include "Networking.h"

//Temp
ManageConnection::ManageConnection() : socket(ioContext), resolver(ioContext) {}

ManageConnection::~ManageConnection() {
ShutitngDown();
}

bool ManageConnection::Initialize() {
	return true;
}

void ManageConnection::ShuttingDown() {
	if (socket.is_open()) {
		socket.close();
	}
}
bool ManageConnection::Connecting() {
	return true;
}
void ManageConnection::Disconnecting() {}

bool ManageConnection::Send() {
	return true;
}
