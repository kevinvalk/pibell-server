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
	void doCall(bool global);

	// Properties
	void setOpenHandler(std::function<bool()> handler);
	void setCloseHandler(std::function<bool()> handler);
	void setPacketHandler(std::function<bool(Packet*)> handler);

private:
	void doReceive();
	void doSend();
	void handleReceive();

	tcp::socket socket_;
	Packet receive_;
	PacketQueue sends_;

	// Open/Close/Receive handlers
	std::function<bool()> handleOpen;
	std::function<bool()> handleClose;
	std::function<bool(Packet*)> handlePacket;
};
