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
	for (auto it = bellClients_.cbegin(); it != bellClients_.cend(); ++it)
	{
		bellClients_.erase(it);
	}

	return true;
}

bool Server::handlePacket(std::shared_ptr<Client> client, Packet* packet)
{
	switch (packet->getType())
	{
		case PacketType::CALL:
			std::cout << "Call packet" << std::endl;
		break;
		case PacketType::REGISTER:
			std::cout << "Register packet" << std::endl;
		break;
		default:
			std::cout << "Received unhandeled packet with type: " << packet->getType() << std::endl;
		break;
	}

	return true;
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
			}
		}
		#endif 
		
		// Always wait, but to avoid SPAM wait more when a button has been pressed
		std::this_thread::sleep_for(std::chrono::milliseconds(isPressed ? 250 : 25));
	}
}
