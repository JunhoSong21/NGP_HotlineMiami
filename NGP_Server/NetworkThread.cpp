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
	char buf[BUFSIZE];
	int retValue;
}

void NetworkThread::PacketProcess()
{

}