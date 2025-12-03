#pragma once
#include <thread>
#include <memory>

#include "Common.h"
#include "Packet.h"
#include "EventQueue.h"
#include "concurrentqueue.h"
#include "DataManager.h"

#include "Player.h"

class NetworkThread {
private:
	int			threadId;
	SOCKET		clientSock;
	std::thread networkThread;

	moodycamel::ConcurrentQueue<int> sendQueue;

	void ThreadFunc();

public:
	NetworkThread(int id, SOCKET clientSock);
	~NetworkThread();

	void LoginProcess();

	void KeyInputPacketProcess(struct CS_KEY_INPUT packet);
	void BulletTriggerPacketProcess(struct CS_BULLET_TRIGGER packet);
	void GrenadeThrowPacketProcess(struct CS_GRENADE_THROW packet);

	void SendQueueInput(int eventNum);

	void SendPlayerMove();
	void SendBulletState();
	void SendGrenadeState();
};