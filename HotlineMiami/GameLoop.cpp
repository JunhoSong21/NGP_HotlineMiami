#include "pch.h"
#include "GameLoop.h"

#include "BackGround.h"
#include "Timer.h"
#include "Player.h"

GameLoop::GameLoop() :
	backGround(nullptr),
	timer(nullptr),
	player(nullptr),
	deltaTime(0.0f),
	hWnd(nullptr)
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
	// 객체 클래스 Init, 이미지 불러오기
	backGround = new BackGround();

	timer = new Timer();

	player = new Player();
	player->Init();
	player->LoadPlayerImages(imgManager);
}

void GameLoop::Update()
{
	if (backGround) backGround->Update();
	if (timer) deltaTime = timer->getDeltaTime();
	if (player) player->Update(deltaTime);
}

void GameLoop::Render()
{
	HDC hDC = GetDC(hWnd);
	if (!hDC)
		DEBUG_MSG(L"화면 HDC 획득 실패");

	RECT clientRect;
	GetClientRect(this->hWnd, &clientRect);
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;
	if (width <= 0 || height <= 0)
		DEBUG_MSG(L"width 혹은 height 값이 0 이하입니다");

	Gdiplus::Bitmap* backBufferBitmap = new Gdiplus::Bitmap(width, height,
		PixelFormat32bppARGB);
	if (!backBufferBitmap || backBufferBitmap->GetLastStatus() != Gdiplus::Ok) {
		DEBUG_MSG(L"백 버퍼 Bitmap 생성 실패");
	}

	{
		Gdiplus::Graphics memoryGraphics(backBufferBitmap);
		memoryGraphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
		memoryGraphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
		memoryGraphics.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
		memoryGraphics.Clear(Gdiplus::Color(0, 0, 0, 0));

		// 각 객체 Render할 공간
		if (backGround) backGround->Render(hWnd, memoryGraphics);
		if (player) player->Render(hWnd, memoryGraphics, imgManager);
	}

	Gdiplus::Graphics screenGraphics(hDC);
	screenGraphics.DrawImage(backBufferBitmap, 0, 0, width, height);

	delete backBufferBitmap;
	ReleaseDC(hWnd, hDC);
}

void GameLoop::InputProcessing(UINT Msg, WPARAM wParam, LPARAM lParam)
{
}