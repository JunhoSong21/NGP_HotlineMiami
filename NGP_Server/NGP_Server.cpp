#include "Common.h"

#define SERVERPORT 9000
#define BUFSIZE    512

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void DisplayText(const char* fmt, ...);
void DisplayError(const char* msg);
DWORD WINAPI ServerMain(LPVOID arg);
DWORD WINAPI ProcessClient(LPVOID arg);

HINSTANCE			hInst; // 인스턴스 핸들
HWND				hEdit; // 에디트 컨트롤
CRITICAL_SECTION	cs; // 임계 영역

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpCmdLine, int nCmdShow)
{
	hInst = hInstance;
	InitializeCriticalSection(&cs);

	// 윈도우 클래스 등록
	WNDCLASS wndclass;
	wndclass.style			= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc	= WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= _T("MyWndClass");
	if (!RegisterClass(&wndclass))
		return 1;

	HWND hWnd = CreateWindow(_T("MyWndClass"), _T("TCP Server"), WS_OVERLAPPEDWINDOW,
		0, 0, 500, 220, NULL, NULL, hInstance, NULL);
	if (hWnd == NULL)
		return 1;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	CreateThread(NULL, 0, ServerMain, NULL, 0, NULL);

	MSG msg;
	while (GetMessage(&msg, 0, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	DeleteCriticalSection(&cs);
	return (int)msg.wParam;
}

// 윈도우 프로시저
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg) {
	case WM_CREATE:
		hEdit = CreateWindow(_T("edit"), NULL,
			WS_CHILD | WS_VISIBLE | WS_HSCROLL |
			WS_VSCROLL | ES_AUTOHSCROLL |
			ES_AUTOVSCROLL | ES_MULTILINE | ES_READONLY,
			0, 0, 0, 0, hWnd, (HMENU)100, hInst, NULL);
		return 0;
	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
	case WM_SETFOCUS:
		SetFocus(hEdit);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// 에디트 컨트롤 출력 함수
void DisplayText(const char* fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);
	char cbuf[BUFSIZE * 2];
	vsprintf(cbuf, fmt, arg);
	va_end(arg);

	EnterCriticalSection(&cs);
	int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	SendMessageA(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);
	LeaveCriticalSection(&cs);
}

// 소켓 함수 오류 출력
void DisplayError(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(char*)&lpMsgBuf, 0, NULL);
	DisplayText("[%s] %s\r\n", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

// TCP 서버 시작 부분
DWORD WINAPI ServerMain(LPVOID arg)
{
	int returnVal;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSock == INVALID_SOCKET) err_quit("socket()");

	// bind()
	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(SERVERPORT);
	returnVal = bind(listenSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
	if (returnVal == SOCKET_ERROR)
		err_quit("bind()");

	// listen()
	returnVal = listen(listenSock, SOMAXCONN);
	if (returnVal == SOCKET_ERROR)
		err_quit("listen()");

	// 데이터 통신에 사용할 변수
	SOCKET clientSock;
	struct sockaddr_in clientAddr;
	int addrLen;
	HANDLE hThread;

	while (true) {
		// accept()
		addrLen = sizeof(clientAddr);
		clientSock = accept(listenSock, (struct sockaddr*)&clientAddr, &addrLen);
		if (clientSock == INVALID_SOCKET) {
			DisplayError("accept()");
			break;
		}

		// 접속한 클라이언트 정보 출력
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clientAddr.sin_addr, addr, sizeof(addr));
		DisplayText("\r\n[TCP 서버] 클라이언트 접속: IP 주소=%s, 포트 번호=%d\r\n",
			addr, ntohs(clientAddr.sin_port));

		// 스레드 생성
		hThread = CreateThread(NULL, 0, ProcessClient,
			(LPVOID)clientSock, 0, NULL);
		if (hThread == NULL) { closesocket(clientSock); }
		else { CloseHandle(hThread); }
	}

	// 소켓 닫기
	closesocket(listenSock);

	// 윈속 종료
	WSACleanup();
	return 0;
}

// 클라이언트와 데이터 통신
DWORD WINAPI ProcessClient(LPVOID arg)
{
	int returnVal;
	SOCKET clientSock = (SOCKET)arg;
	struct sockaddr_in clientAddr;
	char addr[INET_ADDRSTRLEN];
	int addrLen;
	char buf[BUFSIZE + 1];

	// 클라이언트 정보 얻기
	addrLen = sizeof(clientAddr);
	getpeername(clientSock, (struct sockaddr*)&clientAddr, &addrLen);
	inet_ntop(AF_INET, &clientAddr.sin_addr, addr, sizeof(addr));

	while (true) {
		// 데이터 받기
		returnVal = recv(clientSock, buf, BUFSIZE, 0);
		if (returnVal == SOCKET_ERROR) {
			DisplayError("recv()");
			break;
		}
		else if (returnVal == 0)
			break;

		// 받은 데이터 출력
		buf[returnVal] = '\0';
		DisplayText("[TCP/%s:%d] %s\r\n", addr, ntohs(clientAddr.sin_port), buf);

		// 데이터 보내기
		returnVal = send(clientSock, buf, returnVal, 0);
		if (returnVal == SOCKET_ERROR) {
			DisplayError("send()");
			break;
		}
	}

	// 소켓 닫기
	closesocket(clientSock);
	DisplayText("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\r\n",
		addr, ntohs(clientAddr.sin_port));
	return 0;
}