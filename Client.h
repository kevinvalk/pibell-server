#pragma once

#include "Common.h"

class Client : public std::enable_shared_from_this<Client>
{
public:
	Client(tcp::socket socket);
	~Client();

	void start();
	void stop();

	void send(Packet packet);
	void call(bool global);

	// Properties
	void setOpenHandler(std::function<bool(std::shared_ptr<Client>)> handler);
	void setCloseHandler(std::function<bool(std::shared_ptr<Client>)> handler);
	void setPacketHandler(std::function<bool(std::shared_ptr<Client>, Packet*)> handler);

	int bellNo;
	bool bellGlobal;
	bool isRegistered;

private:
	void doReceive();
	void doSend();
	void handleReceive();

	tcp::socket socket_;
	Packet receive_;
	PacketQueue sends_;

	// Open/Close/Receive handlers
	std::function<bool(std::shared_ptr<Client>)> handleOpen;
	std::function<bool(std::shared_ptr<Client>)> handleClose;
	std::function<bool(std::shared_ptr<Client>, Packet*)> handlePacket;
};
