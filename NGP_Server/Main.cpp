#include "Common.h"
#include "resource.h"

constexpr int SERVERPORT = 9000;

HWND hDlg = nullptr;
HWND hEdit1, hEdit2, hEdit3, hEdit4;
HINSTANCE hInst = GetModuleHandleW(NULL);
HBRUSH editControlBrush = nullptr;
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam);

int main(int argc, char* argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;
	else
		printf("NGP_Server Run\n");

	int retValue;

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
	else {
		printf("listen() complete\n");
		hDlg = CreateDialogW(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DialogProc);
		if (hDlg) {
			ShowWindow(hDlg, SW_SHOW);
			
		}
	}
	
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
		else
			printf("client%d accept() complete\n", ++clientNum);

		// 쓰레드 생성 및 데이터 넘겨주기
	}

	closesocket(listenSock);
	WSACleanup();
	return 0;
}

INT_PTR CALLBACK DialogProc(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:
		if (editControlBrush == nullptr)
			editControlBrush = CreateSolidBrush(RGB(255, 255, 255));
		SetTimer(hDlg, 1, 1000, NULL);
		hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);
		hEdit2 = GetDlgItem(hDlg, IDC_EDIT2);
		hEdit3 = GetDlgItem(hDlg, IDC_EDIT3);
		hEdit4 = GetDlgItem(hDlg, IDC_EDIT4);
		return TRUE;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case ID_SHUTDOWN:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		return FALSE;

	case WM_CTLCOLOREDIT:
		SetBkMode((HDC)wParam, OPAQUE);
		SetTextColor((HDC)wParam, RGB(0, 0, 0));
		return (INT_PTR)editControlBrush;

	case WM_TIMER:
		return TRUE;
	case WM_DESTROY:
		return TRUE;
	}

	return FALSE;
}