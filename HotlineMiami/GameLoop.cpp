#include "pch.h"
#include "GameLoop.h"

GameLoop::GameLoop() :
	backGround(nullptr),
	map(nullptr),
	wall(nullptr),
	timer(nullptr),
	player(nullptr),
	grenade(nullptr),
	hud(nullptr),
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

	grenade = new Grenade();
	grenade->Init();
	grenade->LoadGrenadeImage(imgManager);

	// 수류탄에 벽 정보 넘겨주기
	if (wall && grenade)
		grenade->SetWall(wall);

	hud = new HUD();
	hud->Init(imgManager);
}

void GameLoop::Update()
{
	if (backGround) backGround->Update();
	if (timer) deltaTime = timer->getDeltaTime();
	if (player)
	{
		Gdiplus::PointF oldPos = player->GetPos();

		player->Update(deltaTime);

		Gdiplus::PointF newPos = player->GetPos();
		Gdiplus::PointF delta(newPos.X - oldPos.X,
			newPos.Y - oldPos.Y);

		Gdiplus::SizeF playerAabb(50.0f, 50.0f);

		Gdiplus::PointF resolvedPos = oldPos;

		if (wall)
			wall->ResolveMove(resolvedPos, playerAabb, delta);

		player->GetPos() = resolvedPos;
	}
	if (grenade) {
		grenade->Update(deltaTime);
		// 파편 충돌 처리 (벽 + 플레이어)
		grenade->Collision(deltaTime, player);
	}
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

		// 1) backGround
		{
			auto s = g.Save();
			g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			if (backGround) backGround->Render(hWnd, g); 
			g.Restore(s);
		}

		// 2) map
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
		if (grenade) grenade->Render(g, imgManager);
		if (hud) hud->Render(hWnd, g, imgManager);

		// 3) player
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
	switch (Msg)
	{
	case WM_RBUTTONDOWN:
	{
		// 플레이어/수류탄이 준비되어 있을 때만 처리
		if (!player || !grenade)
			return;

		// 마우스 클릭 지점 (클라이언트 좌표계)
		int mouseX = static_cast<short>(LOWORD(lParam));
		int mouseY = static_cast<short>(HIWORD(lParam));

		Gdiplus::PointF startPos = player->GetPos();                 // 플레이어 중심
		Gdiplus::PointF targetPos(
			static_cast<float>(mouseX),
			static_cast<float>(mouseY)
		);

		// 수류탄 투척 시도 (내부에서 2발 제한 체크)
		grenade->Throw(startPos, targetPos);
		break;
	}
	default:
		break;
	}
}
