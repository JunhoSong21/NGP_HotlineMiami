#include "pch.h"
#include "GameLoop.h"

ULONG_PTR gdiplusToken = 0;

HINSTANCE hInst;
LPCTSTR IpszClass = L"Hotline Miami";
LPCTSTR IpszWindowName = L"Hotline Miami.exe";

GameLoop* gameLoop = nullptr;

LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

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
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return (DefWindowProc(hWnd, iMessage, wParam, lParam));
}