#pragma once
#include "Common.h"
#include "ThreadManager.h"
#include "PopEvent.h"

int main()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	else
		printf("NGP_TCP_Server Run\n");

	int retValue = 0;

	// socket
	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET)
		err_quit("socket()");
	else
		printf("socket()			complete\n");

	// SO_KEEPALIVE
	DWORD optValue = 1;
	retValue = setsockopt(listenSock, SOL_SOCKET, SO_KEEPALIVE,
		(const char*)&optValue, sizeof(optValue));
	if (retValue == SOCKET_ERROR)
		err_quit("setsockopt() SO_KEEPALIVE");
	else
		printf("setsockopt() SO_KEEPALIVE	complete\n");

	// SO_REUSEADDR
	optValue = 1;
	retValue = setsockopt(listenSock, SOL_SOCKET, SO_REUSEADDR,
		(const char*)&optValue, sizeof(optValue));
	if (retValue == SOCKET_ERROR)
		err_quit("setsockopt()");
	else
		printf("setsockopt() SO_REUSEADDR	complete\n");

	// bind
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);
	retValue = bind(listenSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retValue == SOCKET_ERROR)
		err_quit("bind()");
	else
		printf("bind()				complete\n");

	// listen
	retValue = listen(listenSock, SOMAXCONN);
	if (retValue == SOCKET_ERROR)
		err_quit("listen()");
	else
		printf("listen()			complete\n");

	SOCKET clientSock{};
	struct sockaddr_in clientAddr{};
	int addrLen = sizeof(clientAddr);

	while (true) {
		// accept
		clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &addrLen);
		if (clientSock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
		else {
			printf("Client accept()			complete\n");
			ThreadManager::GetInstance().AddThread(clientSock);
			printf("Add NetworkThread\n");
		}

		if (ThreadManager::GetInstance().ThreadCount() == 3) {
			printf("3 Client Accept			Complete\n");
			closesocket(listenSock);
			break;
		}
	}

	PopEvent gameLoop;
	while (true) {
		std::unique_ptr<GameEvent> event;
		if (EventQueue::GetInstance().PopEvent(event))
			gameLoop.HandleEvent(std::move(event));
	}
	
	WSACleanup();
	return 0;
}