#include "NetworkThread.h"

using std::unique_ptr;
using std::make_unique;

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
	PacketHeader recvPacketHeader{};

	while (true) {
		retValue = recv(clientSock, (char*)&recvPacketHeader, sizeof(recvPacketHeader), MSG_WAITALL);
		if (retValue == SOCKET_ERROR) {
			err_display("recv()");
			break;
		}

		switch (recvPacketHeader.packetType) {
		case PN::CS_KEY_INPUT: {
			CS_KEY_INPUT keyinputPacket;
			retValue = recv(clientSock, (char*)&keyinputPacket, sizeof(keyinputPacket), MSG_WAITALL);
			if (retValue == SOCKET_ERROR)
				err_display("recv() Ket Input");
			else
				KeyInputPacketProcess(keyinputPacket);
			break;
		}
		case PN::CS_BULLET_TRIGGER: {
			CS_BULLET_TRIGGER bulletTriggerPacket;
			retValue = recv(clientSock, (char*)&bulletTriggerPacket, sizeof(bulletTriggerPacket), MSG_WAITALL);
			if (retValue == SOCKET_ERROR)
				err_display("recv() BulletTrigger");
			else
				BulletTriggerPacketProcess(bulletTriggerPacket);
			break;
		}
		case PN::CS_GRENADE_THROW: {
			CS_GRENADE_THROW grenadeThrowPacket;
			retValue = recv(clientSock, (char*)&grenadeThrowPacket, sizeof(grenadeThrowPacket), MSG_WAITALL);
			if (retValue == SOCKET_ERROR)
				err_display("recv() GrenadeThrow");
			else
				GrenadeThrowPacketProcess(grenadeThrowPacket);
			break;
		}
		case PN::CS_LOGIN_PACKET:
			break;
		case PN::CS_ROOM_PACKET:
			break;
		}

		// send큐에 데이터가 있다면 전송
		int eventNum;
		if (sendQueue.try_dequeue(eventNum)) {
			PacketHeader sendPacketHeader{};
			sendPacketHeader.packetType = eventNum;

			switch (sendPacketHeader.packetType) {
			case PN::SC_PLAYER_MOVE:
				sendPacketHeader.packetSize = sizeof(SC_PLAYER_MOVE);
				retValue = send(clientSock, (char*)&sendPacketHeader, sizeof(sendPacketHeader), 0);
				if (retValue == SOCKET_ERROR)
					printf("send() SC_PLAYER_MOVE() Error\n");
				else
					SendPlayerMove();
				break;
			default:
				break;
			}

			retValue;

		}
	}
}

void NetworkThread::KeyInputPacketProcess(struct CS_KEY_INPUT packet)
{
#ifdef _DEBUG
	printf("Key Input Packet recv %f, %f\n", packet.posX, packet.posY);
#endif
	unique_ptr<GameEvent> playerMoveEvent = make_unique<PlayerMove>(
		threadId, packet.posX, packet.posY, packet.mouseX, packet.mouseY);
	EventQueue::GetInstance().PushEvent(std::move(playerMoveEvent));
}

void NetworkThread::BulletTriggerPacketProcess(struct CS_BULLET_TRIGGER packet)
{
	//auto bulletTriggerEvent = std::make_unique<BulletTrigger>(threadId, 0.0, 0.0);
	//EventQueue::GetInstance().PushEvent(std::move(bulletTriggerEvent));
}

void NetworkThread::GrenadeThrowPacketProcess(struct CS_GRENADE_THROW packet)
{
#ifdef _DEBUG
	printf("Greanade Throw Packet recv\n");
#endif
	unique_ptr<GameEvent> grenadeThrowEvent = make_unique<GrenadeThrow>(
	threadId, packet.dirRadAngle);
	EventQueue::GetInstance().PushEvent(std::move(grenadeThrowEvent));
}

void NetworkThread::SendQueueInput(int eventNum)
{
	switch (eventNum) {
	case GameEvent::Type::PLAYER_UPDATE:
		sendQueue.enqueue(PN::SC_PLAYER_MOVE);
		break;
	default:
		break;
	}
}

void NetworkThread::SendPlayerMove()
{
	int retValue = 0;
	SC_PLAYER_MOVE playerMovePacket{};

	for (int i = 0; i < 1; ++i) {
		playerMovePacket.targetNum = i;
		Player* sendPlayer = DataManager::GetInstance().GetPlayer(i);
		playerMovePacket.posX = sendPlayer->posX;
		playerMovePacket.posY = sendPlayer->posY;
		playerMovePacket.angle = sendPlayer->angle;

		retValue = send(clientSock, (char*)&playerMovePacket, sizeof(playerMovePacket), 0);
		if (retValue == SOCKET_ERROR)
			printf("playerMovePacket Send() Error\n");
		else
			printf("playerMovePacket %f, %f\n", playerMovePacket.posX, playerMovePacket.posY);
	}
}