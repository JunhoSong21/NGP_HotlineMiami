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
				err_display("recv()");
			else
				KeyInputPacketProcess(keyinputPacket);
			break;
		case PN::CS_BULLET_TRIGGER:
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
	
}