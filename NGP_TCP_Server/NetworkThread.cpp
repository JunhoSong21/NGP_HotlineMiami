#include "NetworkThread.h"

using std::unique_ptr;
using std::make_unique;

NetworkThread::NetworkThread(int id, SOCKET clientSock) :
	threadId(id), clientSock(clientSock)
{
	networkThread = std::thread(&NetworkThread::ThreadFunc, this);
	printf("Client %d Thread Start\n", threadId);

	// Nagle Algorithm
	int optValue = 1;
	setsockopt(clientSock, IPPROTO_TCP, TCP_NODELAY, (const char*)& optValue, sizeof(optValue));
	printf("Client %d Nalge Algorithm Off\n", threadId);
}

NetworkThread::~NetworkThread()
{
	if (networkThread.joinable())
		networkThread.join();

	closesocket(clientSock);
}

void NetworkThread::LoginProcess()
{
	int retValue = 0;
	PacketHeader loginHeader{};

	// recv
	retValue = recv(clientSock, (char*)&loginHeader, sizeof(loginHeader), MSG_WAITALL);
	if (retValue == SOCKET_ERROR)
		printf("Login Packet recv() Error\n");

	if (loginHeader.packetType == PN::CS_LOGIN_PACKET) {
		CS_LOGIN_PACKET loginPacket;
		retValue = recv(clientSock, (char*)&loginPacket, sizeof(loginPacket), MSG_WAITALL);
		if (retValue == SOCKET_ERROR)
			printf("loginPacket recv Error\n");

		unique_ptr<Player> newPlayer = make_unique<Player>(threadId);
		DataManager::GetInstance().AddPlayer(std::move(newPlayer));
	}

	// send
	PacketHeader loginSendHeader{};
	loginSendHeader.packetType = PN::SC_LOGIN_SUCCESS;
	loginSendHeader.packetSize = sizeof(SC_LOGIN_SUCCESS);
	retValue = send(clientSock, (char*)&loginSendHeader, sizeof(loginSendHeader), 0);
	if (retValue == SOCKET_ERROR)
		printf("Login Success Packet Header send() Error\n");

	SC_LOGIN_SUCCESS loginSuccessPacket{true, threadId};
	retValue = send(clientSock, (char*)&loginSuccessPacket, sizeof(loginSuccessPacket), 0);
	if (retValue == SOCKET_ERROR)
		printf("Login Success Packet send() Error\n");
}

void NetworkThread::ThreadFunc()
{
	LoginProcess();

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
		case PN::CS_ROOM_PACKET:
			break;
		}

		// send큐에 데이터가 있다면 전송
		int eventNum;
		if (sendQueue.try_dequeue(eventNum)) {
			PacketHeader sendPacketHeader{};
			sendPacketHeader.packetType = eventNum;

			switch (sendPacketHeader.packetType) {
			case PN::SC_PLAYER_MOVE: {
				sendPacketHeader.packetSize = sizeof(SC_PLAYER_MOVE);
				retValue = send(clientSock, (char*)&sendPacketHeader, sizeof(sendPacketHeader), 0);
				if (retValue == SOCKET_ERROR)
					printf("send() SC_PLAYER_MOVE() Error\n");
				else
					SendPlayerMove();
				break;
			}
			case PN::SC_GRENADE_STATE: {
				sendPacketHeader.packetSize = sizeof(SC_GRENADE_STATE);
				retValue = send(clientSock, (char*)&sendPacketHeader, sizeof(sendPacketHeader), 0);
				if (retValue == SOCKET_ERROR)
					printf("send() SC_GRENADE_STATE() Error\n");
				else
					SendGrenadeState();
				break;
			}
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
#ifdef _DEBUG
	printf("Bullet Trigger Packet recv\n");
#endif
	unique_ptr<GameEvent> bulletTriggerEvent = make_unique<BulletTrigger>(
	threadId, packet.posX, packet.posY, packet.dirRadAngle);
	EventQueue::GetInstance().PushEvent(std::move(bulletTriggerEvent));
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
	case GameEvent::Type::GRENADE_EXPLOSION:
		sendQueue.enqueue(PN::SC_GRENADE_STATE);
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
		if (sendPlayer) {
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
}

void NetworkThread::SendGrenadeState()
{
	int retValue = 0;
	SC_GRENADE_STATE grenadeStatePacket{};

	grenadeStatePacket.isExploded = true;
	
	retValue = send(clientSock, (char*)&grenadeStatePacket, sizeof(grenadeStatePacket), 0);
	if (retValue == SOCKET_ERROR)
		printf("grenadeStatePacket Send() Error\n");
	else
		printf("grenadeStatePacket Send() Complete\n");
}