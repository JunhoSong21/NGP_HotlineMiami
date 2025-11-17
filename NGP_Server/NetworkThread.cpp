#include "NetworkThread.h"

NetworkThread::NetworkThread(int id, SOCKET clientSock) :
	threadId(id),
	clientSock(clientSock)
{
	clientThread = std::thread(&NetworkThread::ThreadFunc, this);
	printf("Client %d Thread Start\n", threadId);
}

NetworkThread::~NetworkThread()
{
	if (clientThread.joinable())
		clientThread.join();

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
			CS_KEY_INPUT keyInputPacket;
			retValue = recv(clientSock, (char*)&keyInputPacket, sizeof(keyInputPacket), MSG_WAITALL);
			if (retValue == SOCKET_ERROR)
				err_display("recv()");
			KeyInputPacketProcess(keyInputPacket);
			break;
		case PN::CS_BULLET_TRIGGER:
			CS_BULLET_TRIGGER bulletTriggerPacket;
			retValue = recv(clientSock, (char*)&bulletTriggerPacket, sizeof(bulletTriggerPacket), MSG_WAITALL);
			if (retValue == SOCKET_ERROR)
				err_display("recv()");
			break;
		case PN::CS_GRENADE_THROW:
			CS_GRENADE_THROW grenadeThrowPacket;
			retValue = recv(clientSock, (char*)&grenadeThrowPacket, sizeof(grenadeThrowPacket), MSG_WAITALL);
			if (retValue == SOCKET_ERROR)
				err_display("recv()");
			break;
		case PN::CS_LOGIN_PACKET:
			CS_LOGIN_PACKET loginPacket;
			retValue = recv(clientSock, (char*)&loginPacket, sizeof(loginPacket), MSG_WAITALL);
			if (retValue == SOCKET_ERROR)
				err_display("recv()");
			break;
		default:
			printf("정의되지 않은 패킷 타입\n");
			break;
		}
	}

	//if(send 큐에 데이터가 있을 경우) {
	//send 작업
	//}
}

void NetworkThread::KeyInputPacketProcess(struct CS_KEY_INPUT)
{

}

void NetworkThread::BulletTriggerPacketProcess(struct CS_BULLET_TRIGGER)
{

}

void NetworkThread::GrenadeThrowPacketProcess(struct CS_GRENADE_THROW)
{

}