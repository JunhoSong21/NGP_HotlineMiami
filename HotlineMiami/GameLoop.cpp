#include "pch.h"
#include "GameLoop.h"

GameLoop::GameLoop() :
	backGround(nullptr),
	map(nullptr),
	wall(nullptr),
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
        
	map = new Map();
	map->LoadMapImages(imgManager);
	map->Init();

	wall = new Wall();
	wall->LoadImages(imgManager);
	wall->Init();

	for (int i = 0; i < 10; ++i)
		wall->AddWall(Wall::BrickH, i, 0, 1, 1, Wall::DrawMode::Tiled);

	for (int i = 0; i < 10; ++i)
		wall->AddWall(Wall::BrickV, 0, i, 1, 1, Wall::DrawMode::Tiled);
	
	wall->AddWall(Wall::BarBooth, 3, 4, 1, 1, Wall::DrawMode::Sprite);
	wall->AddWall(Wall::BossSofa, 15, 10, 3, 2, Wall::DrawMode::Sprite);
	wall->AddWall(Wall::PoolTable, 10, 6, 2, 3, Wall::DrawMode::Sprite);
	wall->AddWall(Wall::BigBed, 6, 10, 1, 1, Wall::DrawMode::Sprite);

	timer = new Timer();

	player = new Player();
	player->Init();
	player->LoadPlayerImages(imgManager);    
}

void GameLoop::Update()
{
	if (backGround) backGround->Update();
	if (timer) deltaTime = timer->getDeltaTime();
	if (player)
	{
		// 1) 이동 전 위치
		Gdiplus::PointF oldPos = player->GetPos();

		// 2) 플레이어 자체 로직으로 먼저 이동 시도
		player->Update(deltaTime);

		// 3) 이동 후 위치와 delta 계산
		Gdiplus::PointF newPos = player->GetPos();
		Gdiplus::PointF delta(newPos.X - oldPos.X,
			newPos.Y - oldPos.Y);

		// 렌더 기준 실제 충돌 박스 크기
		Gdiplus::SizeF playerAabb(90.0f, 50.0f);

		// 4) 충돌 처리용 위치는 oldPos에서 시작
		Gdiplus::PointF resolvedPos = oldPos;

		if (wall)
			wall->ResolveMove(resolvedPos, playerAabb, delta);

		// 5) 충돌 처리 결과를 플레이어에 반영
		player->GetPos() = resolvedPos;
	}
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

	Gdiplus::Bitmap* backBufferBitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
	if (!backBufferBitmap || backBufferBitmap->GetLastStatus() != Gdiplus::Ok) {
		DEBUG_MSG(L"백 버퍼 Bitmap 생성 실패");
	}

	{
		Gdiplus::Graphics g(backBufferBitmap);
		g.Clear(Gdiplus::Color(0, 0, 0, 0));

		// 1) 배경 
		{
			auto s = g.Save();
			g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			if (backGround) backGround->Render(hWnd, g); 
			g.Restore(s);
		}

		// 2) 맵(타일) 
		{
			auto s = g.Save();
			g.SetSmoothingMode(Gdiplus::SmoothingModeNone);
			g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
			if (map) map->Render(g);
			g.Restore(s);
		}

		if (wall) wall->Render(g);     

		// 3) 플레이어
		{
			auto s = g.Save();
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
			g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
			if (player) player->Render(hWnd, g, imgManager);
			g.Restore(s);
		}
	}

	Gdiplus::Graphics screen(hDC);
	screen.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
	screen.DrawImage(backBufferBitmap, 0, 0, width, height);

	delete backBufferBitmap;
	ReleaseDC(hWnd, hDC);
}

void GameLoop::InputProcessing(UINT Msg, WPARAM wParam, LPARAM lParam)
{
}

