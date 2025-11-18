#pragma once
#include <thread>

#include "Common.h"
#include "Packet.h"
#include "EventQueue.h"

class NetworkThread {
private:
	int			threadId;
	SOCKET		clientSock;
	std::thread networkThread;

	void ThreadFunc();

public:
	NetworkThread(int id, SOCKET clientSock);
	~NetworkThread();

	void KeyInputPacketProcess(struct CS_KEY_INPUT packet);
};