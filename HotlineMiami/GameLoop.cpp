#include "pch.h"
#include "GameLoop.h"

#include "Timer.h"
#include "Player.h"

GameLoop::GameLoop() :
	hWnd(nullptr),
	timer(nullptr),
	player(nullptr),
	deltaTime(0.0f)
{
}

GameLoop::~GameLoop()
{
	if (player) {
		delete player;
		player = nullptr;
	}
}

void GameLoop::Init(HWND hwnd)
{
	hWnd = hwnd;

	timer = new Timer();

	POINT playerPosition{ 500, 500 };
	player = new Player(playerPosition);
	player->Init();
}

void GameLoop::Update()
{
	if (timer) deltaTime = timer->getDeltaTime();
	if (player) player->Update(deltaTime);
	if (player) player->InputProcessing(deltaTime);
}

void GameLoop::Render()
{
	HDC hDC = GetDC(this->hWnd);

	RECT clientRect;
	GetClientRect(this->hWnd, &clientRect);
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;

	HDC memoryDC = CreateCompatibleDC(hDC);
	HBITMAP hBitmap = CreateCompatibleBitmap(hDC, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(memoryDC, hBitmap);

	if (player) player->Render(hWnd, memoryDC);

	BitBlt(hDC, 0, 0, width, height, memoryDC, 0, 0, SRCCOPY);

	SelectObject(memoryDC, oldBitmap);
	DeleteObject(hBitmap);
	DeleteDC(memoryDC);
	ReleaseDC(this->hWnd, hDC);
}

void GameLoop::InputProcessing(UINT Msg, WPARAM wParam, LPARAM lParam)
{
	//if (player) player->InputProcessing();
}