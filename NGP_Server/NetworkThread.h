#pragma once
#include <thread>
#include <concurrent_queue.h>
#include "Common.h"
#include "Packet.h"

using namespace Concurrency;

class NetworkThread {
private:
	int			threadId;	// 클라이언트 고유 id값
	SOCKET		clientSock;
	std::thread clientThread;

	concurrent_queue<int> ThreadQueue;

	void ThreadFunc();

public:
	NetworkThread(int id, SOCKET clientSock);
	~NetworkThread();

	void KeyInputPacketProcess(struct CS_KEY_INPUT);
	void BulletTriggerPacketProcess(struct CS_BULLET_TRIGGER);
	void GrenadeThrowPacketProcess(struct CS_GRENADE_THROW);
};

