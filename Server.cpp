#include "Server.h"

Server::Server()
{

}


Server::~Server()
{

}

void Server::start()
{
	// Start main thread
	isRunning_ = true;
	thread_ = std::thread(&Server::run, this);
}

void Server::stop()
{
	std::cout << "Closing the server" << std::endl;

	// Wait for our thread to exit
	isRunning_ = false;
	thread_.join();
}

bool Server::call(int bellNo)
{
	try
	{
		for (auto client : bellClients_.at(bellNo))
		{
			client->call(false);
		}
		return true;
	}
	catch(std::out_of_range& e)
	{
		return false;
	}
}

void Server::callGlobal()
{
	for (auto client : clients_)
	{
		if (client->isRegistered && client->bellGlobal)
		{
			client->call(true);
		}
	}
}

bool Server::handleOpen(std::shared_ptr<Client> client)
{
	std::cout << "Handle open" << std::endl;
	clients_.push_back(client);
	return true;
}

bool Server::handleClose(std::shared_ptr<Client> client)
{
	std::cout << "Handle close" << std::endl;	

	// Remove the client from our management
	clients_.remove(client);

	// Remove the client from registered bells
	for (auto& bellClient : bellClients_)
	{
		bellClient.second.remove(client);
		if (bellClient.second.empty())
			bellClients_.erase(bellClient.first);
	}

	return true;
}

bool Server::handlePacket(std::shared_ptr<Client> client, Packet* packet)
{
	switch (packet->getType())
	{
		case REGISTER: return handleRegister(client, packet);
		default:
			std::cout << "Received unhandeled packet with type: " << packet->getType() << std::endl;
			return false;
	}
}

bool Server::handleRegister(std::shared_ptr<Client> client, Packet* packet)
{
	auto p = reinterpret_cast<PacketRegister*>(packet);

	if (std::string(p->username.get()) == "kevinvalk" && std::string(p->password.get()) == "asdasdasd")
	{
		std::cout << "Succesfull login for " << p->username.get() << std::endl;
		client->bellNo = 0;
		client->bellGlobal = p->global;
		client->isRegistered = true;
		bellClients_[client->bellNo].push_back(client);

		// State OK
		PacketState state(true);
		client->send(PacketCast state);


		return true;
	}
	else
	{
		std::cout << "Login failed for " << p->username.get() << std::endl;
		return false;
	}
}

void Server::run()
{
	#ifdef RASPBERRY
	if(wiringPiSetup() < 0)
	{
		std::cerr << "Could not initialize wiringPi" << std::endl;
		exit(1);
	}
	#endif
	
	while (isRunning_)
	{
		bool isPressed = false;

		// If we running cygwin simulate keypresses with 1..7 for clients 0..6 and 0 for global
		#ifdef CYGWIN
		for (char k = 0x30; k < 0x40; ++k)
		{
			if (GetAsyncKeyState(k) & 0x8000)
			{
				unsigned int bellNo = k - 0x30;
				std::cout << "Pressed: " << bellNo << std::endl;
				isPressed = true;

				if (bellNo == 0)
				{
					callGlobal();
				}
				else
				{
					call(bellNo-1);
				}
			}	
		}
		#elif RASPBERRY
		// If we running raspberry use GPIO where 0..6 for clients 0..6 and 7 for global
		for(int bellNo = 0; bellNo < 7; ++bellNo)
		{
			if(digitalRead(bellNo) == HIGH)
			{
				std::cout << "Pressed: " << bellNo << std::endl;
				isPressed = true;
				if (bellNo >= 7)
				{
					callGlobal();
				}
				else
				{
					call(bellNo);
				}
			}
		}
		#endif 
		
		// Always wait, but to avoid SPAM wait more when a button has been pressed
		std::this_thread::sleep_for(std::chrono::milliseconds(isPressed ? 250 : 25));
	}
}
