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
	// recv
	while (true) {

	}

	//if(send 큐에 데이터가 있을 경우) {
	//send 작업
	//}
}

void NetworkThread::PacketProcess()
{

}