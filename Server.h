#pragma once

#include "Common.h"
#include "Client.h"


class Server
{
public:
	Server();
	~Server();

	void start();
	void stop();

	// Client handlers
	bool handleOpen(std::shared_ptr<Client> client);
	bool handleClose(std::shared_ptr<Client> client);
	bool handlePacket(std::shared_ptr<Client> client, Packet* packet);

private:
	void run();

	bool isRunning_;

	std::thread thread_;
	std::list<std::shared_ptr<Client>> clients_;
	std::map<unsigned short, std::list<std::shared_ptr<Client>>> bellClients_;
};
