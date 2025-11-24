#include "NetworkThread.h"

NetworkThread::NetworkThread(int id, SOCKET clientSock) :
	threadId(id), clientSock(clientSock)
{
	networkThread = std::thread(&NetworkThread::ThreadFunc, this);
	printf("Client %d Thread Start\n", threadId);
}

NetworkThread::~NetworkThread()
{
	if (networkThread.joinable())
		networkThread.join();

	closesocket(clientSock);
}

void NetworkThread::ThreadFunc()
{
	int retValue = 0;
	PacketHeader packetHeader{};

	while (true) {
		retValue = recv(clientSock, (char*)&packetHeader, sizeof(packetHeader), MSG_WAITALL);
		if (retValue == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}

		switch (packetHeader.packetType) {
		case PN::CS_KEY_INPUT:
			CS_KEY_INPUT keyinputPacket;
			retValue = recv(clientSock, (char*)&keyinputPacket, sizeof(keyinputPacket), MSG_WAITALL);
			if (retValue == SOCKET_ERROR)
				err_display("recv() Ket Input");
			else
				KeyInputPacketProcess(keyinputPacket);
			break;
		case PN::CS_BULLET_TRIGGER:
			CS_BULLET_TRIGGER bulletTriggerPacket;
			retValue = recv(clientSock, (char*)&bulletTriggerPacket, sizeof(bulletTriggerPacket), MSG_WAITALL);
			if (retValue == SOCKET_ERROR)
				err_display("recv() BulletTrigger");
			else
				BulletTriggerPacketProcess(bulletTriggerPacket);
			break;
		case PN::CS_GRENADE_THROW:
			break;
		case PN::CS_LOGIN_PACKET:
			break;
		case PN::CS_ROOM_PACKET:
			break;
		}
	}
}

void NetworkThread::KeyInputPacketProcess(struct CS_KEY_INPUT packet)
{
	printf("Key Input Packet recv %f, %f\n", packet.posX, packet.posY);
	auto playerMoveEvent = std::make_unique<PlayerMove>(threadId, 0.0, 0.0);
	EventQueue::GetInstance().PushEvent(std::move(playerMoveEvent));
}

void NetworkThread::BulletTriggerPacketProcess(struct CS_BULLET_TRIGGER packet)
{
	//auto bulletTriggerEvent = std::make_unique<BulletTrigger>(threadId, 0.0, 0.0);
	//EventQueue::GetInstance().PushEvent(std::move(bulletTriggerEvent));
}

void NetworkThread::GrenadeThrowPacketProcess(struct CS_GRENADE_THROW packet)
{
	//auto grenadeThrowEvent = std::make_unique<GrenadeThrow>(threadId, 0, 0, 0.0);
	//EventQueue::GetInstance().PushEvent(std::move(grenadeThrowEvent));
}