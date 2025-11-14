#pragma once
#include <thread>
#include "Common.h"
#include "Packet.h"

class NetworkThread {
private:
	int			threadId;	// 클라이언트 고유 id값
	SOCKET		clientSock;
	std::thread clientThread;

	void ThreadFunc();

public:
	NetworkThread(int id, SOCKET clientSock);
	~NetworkThread();

	void KeyInputPacketProcess(struct CS_KEY_INPUT);
	void BulletTriggerPacketProcess(struct CS_BULLET_TRIGGER);
	void GrenadeThrowPacketProcess(struct CS_GRENADE_THROW);
};

