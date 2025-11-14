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
		case PN::SC_PLAYER_MOVE:
			SC_PLAYER_MOVE playerMovePacket;
			retValue = recv(clientSock, (char*)&playerMovePacket, sizeof(playerMovePacket), MSG_WAITALL);
			if (retValue == SOCKET_ERROR)
				err_display("recv()");
			break;
		case PN::SC_BULLET_STATE:
			SC_BULLET_STATE bulletStatePacket;
			retValue = recv(clientSock, (char*)&bulletStatePacket, sizeof(bulletStatePacket), MSG_WAITALL);
			if (retValue == SOCKET_ERROR)
				err_display("recv()");
			break;
		case PN::SC_GRENADE_STATE:
			SC_GRENADE_STATE grenadeStatePacket;
			retValue = recv(clientSock, (char*)&grenadeStatePacket, sizeof(grenadeStatePacket), MSG_WAITALL);
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

void NetworkThread::PacketProcess()
{

}