#include "Common.h"

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
		printf("socket() complete\n");

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
		printf("bind() complete\n");

	// listen
	retValue = listen(listenSock, SOMAXCONN);
	if (retValue == SOCKET_ERROR)
		err_quit("listen()");
	else
		printf("listen() complete\n");

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
		else
			printf("Client accept() complete\n");
	}


	WSACleanup();
	return 0;
}