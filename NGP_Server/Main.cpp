#include "Common.h"

constexpr int SERVERPORT	= 9000;
constexpr int BUFSIZE		= 1024;

int main(int argc, char* argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	int retValue;

	// socket
	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET)
		err_quit("socket()");

	// bind
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);
	retValue = bind(listenSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (retValue == SOCKET_ERROR)
		err_quit("bind()");

	// listen
	retValue = listen(listenSock, SOMAXCONN);
	if (retValue == SOCKET_ERROR)
		err_quit("listen()");

	SOCKET clientSock;
	struct sockaddr_in clientAddr;
	int addrLen = sizeof(clientAddr);
	int clientNum = 0;	// 클라이언트 쓰레드 개수

	while (clientNum < 3) {
		// accept
		clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &addrLen);
		if (clientSock == INVALID_SOCKET) {
			err_display("accept()");
			break;
		}
	}

	closesocket(listenSock);
	WSACleanup();
	return 0;
}