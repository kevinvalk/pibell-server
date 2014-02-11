#pragma once

#include "Common.h"

class Server;

class Client : public std::enable_shared_from_this<Client>
{
public:
	Client(tcp::socket socket, std::shared_ptr<Server> server);
	~Client();

	void start();
	void send(Packet packet);

	void doCall(bool global);

private:
	void doReceive();
	void handleReceive();
	void doSend();

	// Handlers
	bool handleRegister();

	tcp::socket socket_;
	Packet receive_;
	PacketQueue sends_;

	std::shared_ptr<Server> server_;
};
