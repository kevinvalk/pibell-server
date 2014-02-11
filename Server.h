#pragma once

#include "Common.h"
#include "Client.h"

#ifdef RASPBERRY
	#include <wiringPi.h>
#endif

class Server
{
public:
	Server();
	~Server();

	// Start/Stop of the server
	void start();
	void stop();

	bool call(int bellNo);
	void callGlobal();

	// Client handlers
	bool handleOpen(std::shared_ptr<Client> client);
	bool handleClose(std::shared_ptr<Client> client);
	bool handlePacket(std::shared_ptr<Client> client, Packet* packet);

private:
	// All handlers we have
	bool handleRegister(std::shared_ptr<Client> client, Packet* packet);

	// Thread system
	void run();
	bool isRunning_;
	std::thread thread_;

	// Clients
	std::list<std::shared_ptr<Client>> clients_;
	std::map<unsigned short, std::list<std::shared_ptr<Client>>> bellClients_;
};
