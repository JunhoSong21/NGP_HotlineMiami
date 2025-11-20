#include "pch.h"
#include "GameLoop.h"

GameLoop::GameLoop() :
	backGround(nullptr),
	map(nullptr),
	wall(nullptr),
	timer(nullptr),
	player(nullptr),
	bullet(nullptr),
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
	// °´Ã¼ Å¬·¡½º Init, ÀÌ¹ÌÁö ºÒ·¯¿À±â
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

	bullet = new Bullet();
	bullet->LoadBulletImages(imgManager);
	bullet->Init(
		640.0f, 360.0f,   // 시작 위치
		1.0f, 0.0f,       // 진행 방향 (오른쪽)
		0                 // ownerID
	);
}

void GameLoop::Update()
{
	if (backGround) backGround->Update();
	if (timer) deltaTime = timer->getDeltaTime();
	if (player)
	{
		// 1) ÀÌµ¿ Àü À§Ä¡
		Gdiplus::PointF oldPos = player->GetPos();

		// 2) ÇÃ·¹ÀÌ¾î ÀÚÃ¼ ·ÎÁ÷À¸·Î ¸ÕÀú ÀÌµ¿ ½Ãµµ
		player->Update(deltaTime);

		// 3) ÀÌµ¿ ÈÄ À§Ä¡¿Í delta °è»ê
		Gdiplus::PointF newPos = player->GetPos();
		Gdiplus::PointF delta(newPos.X - oldPos.X,
			newPos.Y - oldPos.Y);

		// ·»´õ ±âÁØ ½ÇÁ¦ Ãæµ¹ ¹Ú½º Å©±â
		Gdiplus::SizeF playerAabb(90.0f, 50.0f);

		// 4) Ãæµ¹ Ã³¸®¿ë À§Ä¡´Â oldPos¿¡¼­ ½ÃÀÛ
		Gdiplus::PointF resolvedPos = oldPos;

		if (wall)
			wall->ResolveMove(resolvedPos, playerAabb, delta);

		// 5) Ãæµ¹ Ã³¸® °á°ú¸¦ ÇÃ·¹ÀÌ¾î¿¡ ¹Ý¿µ
		player->GetPos() = resolvedPos;
	}
	if (bullet)     bullet->Update(deltaTime);
}

void GameLoop::Render()
{
	HDC hDC = GetDC(hWnd);
	if (!hDC)
		DEBUG_MSG(L"È­¸é HDC È¹µæ ½ÇÆÐ");

	RECT clientRect;
	GetClientRect(this->hWnd, &clientRect);
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;
	if (width <= 0 || height <= 0)
		DEBUG_MSG(L"width È¤Àº height °ªÀÌ 0 ÀÌÇÏÀÔ´Ï´Ù");

	Gdiplus::Bitmap* backBufferBitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
	if (!backBufferBitmap || backBufferBitmap->GetLastStatus() != Gdiplus::Ok) {
		DEBUG_MSG(L"¹é ¹öÆÛ Bitmap »ý¼º ½ÇÆÐ");
	}

	{
		Gdiplus::Graphics g(backBufferBitmap);
		g.Clear(Gdiplus::Color(0, 0, 0, 0));

		// 1) 배경 렌더링
		{
			auto s = g.Save();
			g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			if (backGround) backGround->Render(hWnd, g); 
			g.Restore(s);
		}

		// 2) 맵(타일) 렌더링
		{
			auto s = g.Save();
			g.SetSmoothingMode(Gdiplus::SmoothingModeNone);
			g.SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
			if (map) map->Render(g);
			g.Restore(s);
		}

		// 3) 벽(Wall) 렌더링
		if (wall) wall->Render(g);     

		// 4) 플레이어 렌더링
		{
			auto s = g.Save();
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
			g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
			if (player) player->Render(hWnd, g, imgManager);
			g.Restore(s);
		}

		// 5) 총알 렌더링
		{
			auto s = g.Save();
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
			g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
			if (bullet) bullet->Render(hWnd, g, imgManager);
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
