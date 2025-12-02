#include "pch.h"
#include "GameLoop.h"
#include "ClientNetwork.h"
#include "resource.h"
ULONG_PTR gdiplusToken = 0;

HINSTANCE hInst;
LPCTSTR IpszClass = L"Hotline Miami";
LPCTSTR IpszWindowName = L"Hotline Miami.exe";

GameLoop* gameLoop = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);
INT_PTR CALLBACK IdDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR IpszCmdParam, int nCmdShow)
{

	Gdiplus::GdiplusStartupInput gdiplusStartUpInput;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartUpInput, NULL);

	HWND hWnd;
	MSG msg;
	WNDCLASSEX WndClass;
	hInst = hInstance;

	WndClass.cbSize			= sizeof(WndClass);
	WndClass.style			= CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc	= (WNDPROC)WndProc;
	WndClass.cbClsExtra		= 0;
	WndClass.cbWndExtra		= 0;
	WndClass.hInstance		= hInstance;
	WndClass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor		= LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	WndClass.lpszMenuName	= NULL;
	WndClass.lpszClassName	= IpszClass;
	WndClass.hIconSm		= LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	hWnd = CreateWindow(IpszClass, IpszWindowName, WS_OVERLAPPEDWINDOW,
						0, 0,
						1280, 800,
						NULL, (HMENU)NULL, hInstance, NULL);
	if (!hWnd)
		return false;

	gameLoop = new GameLoop();
	gameLoop->Init(hWnd);
	INT_PTR ret = DialogBox(
		hInst,
		MAKEINTRESOURCE(IDD_DIALOG),
		hWnd,
		IdDlgProc
	);

	if (!InitNetwork("127.0.0.1", 9000)) {
		MessageBox(hWnd, L"서버 연결 실패", L"Error", MB_OK);
		return -1;
	}


	// 서버통신 스레드 생성
	NetworkThreadParam* netParam = new NetworkThreadParam;
	netParam->hWnd = hWnd;
	netParam->bullet = gameLoop->GetBullet();
	netParam->players = gameLoop->GetPlayers();

	HANDLE hNetThread = CreateThread(
		nullptr,
		0,
		Client_Network_Thread,
		netParam,
		0,
		nullptr
	);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (true) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (WM_QUIT == msg.message)
				break;
			
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		else {
			if (gameLoop) {
				gameLoop->Update();
				gameLoop->Render();
			}

			Sleep(1); // 60 FPS
		}
	}


	Gdiplus::GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	if (gameLoop)
		gameLoop->InputProcessing(iMessage, wParam, lParam);

	switch (iMessage) {
	case WM_CREATE:
		break;
	case WM_PAINT:
		break;
	case WM_SETCURSOR:
		if (!gameLoop->IsTitle()) {
			SetCursor(nullptr);	// 기본 커서 숨기기
		}
		return TRUE;
	case WM_DESTROY:
		g_NetworkRunning = false;
		ShutdownNetwork();
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}

INT_PTR CALLBACK IdDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		// 다이얼로그 뜰 때 에디트박스에 포커스
		SetFocus(GetDlgItem(hDlg, IDC_EDIT_ID));
		return FALSE; // 우리가 포커스 설정했으니 FALSE

	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDC_BTN_OK:
		{
			char buf[20] = {};
			GetDlgItemTextA(hDlg, IDC_EDIT_ID, buf, sizeof(buf));

			if (buf[0] == '\0')
			{
				MessageBoxW(hDlg, L"ID를 입력하세요.", L"알림", MB_OK);
				return TRUE;
			}

			// Send_Input에서 로그인 패킷을 보내도록 플래그 설정
			g_LoginReq.requested = true;
			strncpy_s(g_LoginReq.id, buf, sizeof(g_LoginReq.id) - 1);

			EndDialog(hDlg, IDOK);
			return TRUE;
		}

		}
		break;
	}

	return FALSE;
}