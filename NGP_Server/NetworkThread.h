#pragma once
#include <thread>
#include "Common.h"

class NetworkThread {
private:
	int			threadId;	// 클라이언트 고유 id값
	SOCKET		clientSock;
	std::thread clientThread;

	void ThreadFunc();

public:
	NetworkThread(int id, SOCKET clientSock);
	~NetworkThread();

	void PacketProcess();
};

