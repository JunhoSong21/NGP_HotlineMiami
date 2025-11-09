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
	int retValue;
	PacketHeader header;

	while (true) {
		retValue = recv(clientSock, (char*)&header, sizeof(header), MSG_WAITALL);
		if (retValue == SOCKET_ERROR) {
			err_display("Client Thread recv() Error");
			break;
		}

		printf("Client %d Thread Packet Input\n", threadId);
		
		size_t totalInput = 0;
		size_t bytesToRecv = 0;
		char buf[BUFSIZE];
		while (totalInput < header.size) {
			bytesToRecv = recv(clientSock, buf, (size_t)bytesToRecv, MSG_WAITALL);
			totalInput += retValue;
		}
	}
}

void NetworkThread::PacketProcess()
{

}