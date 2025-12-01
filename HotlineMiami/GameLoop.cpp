#include "pch.h"
#include "GameLoop.h"
#include "ClientNetwork.h"

GameLoop::GameLoop() :
	backGround(nullptr),
	map(nullptr),
	wall(nullptr),
	timer(nullptr),
	grenade(nullptr),
	hud(nullptr),
	bullet(nullptr),
	deltaTime(0.0f),
	hWnd(nullptr)
{
	for (int i = 0; i < 3; ++i)
		players[i] = nullptr;
}

GameLoop::~GameLoop()
{
	for (int i = 0; i < 3; ++i) {
		if (players[i]) {
			delete players[i];
			players[i] = nullptr;
		}
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

	for (int i = 0; i < 3; ++i)
	{
		players[i] = new Player();
		players[i]->Init();
		players[i]->LoadPlayerImages(imgManager);
	}

	grenade = new Grenade();
	grenade->Init();
	grenade->LoadGrenadeImage(imgManager);
	SoundManager::Get().LoadSound("grenade_explosion", "Resource/Sound/GrenadeExplosion.wav");

	// 수류탄에 벽 정보 넘겨주기
	if (wall && grenade)
		grenade->SetWall(wall);

	hud = new HUD();
	hud->Init(imgManager);
	players[0]->LoadPlayerImages(imgManager);

	bullet = new Bullet();
	bullet->LoadBulletImages(imgManager);
	bullet->SetVisible(false);

	OutputDebugStringA("Init!\n");
}

void GameLoop::Update()
{
	if (backGround) backGround->Update();
	if (timer) deltaTime = timer->getDeltaTime();
	// 내 인덱스 계산
	int myIdx = g_MyPlayerIndex;
	if (myIdx < 0 || myIdx >= 3)
		myIdx = 0;


	if (players[myIdx])
	{
		Gdiplus::PointF oldPos = players[myIdx]->GetPos();

		players[myIdx]->Update(deltaTime);

		Gdiplus::PointF newPos = players[myIdx]->GetPos();
		Gdiplus::PointF delta(newPos.X - oldPos.X,
			newPos.Y - oldPos.Y);

		Gdiplus::SizeF playerAabb(50.0f, 50.0f);
		Gdiplus::PointF resolvedPos = oldPos;

		if (wall)
			wall->ResolveMove(resolvedPos, playerAabb, delta);

		players[myIdx]->GetPos() = resolvedPos;
	}

	if (grenade) {
		grenade->Update(deltaTime);

		// 파편 충돌: 내 플레이어 기준
		if (players[myIdx])
			grenade->Collision(deltaTime, players[myIdx]);
	}

	if (bullet)
		bullet->Update(deltaTime);
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

		// 3) Wall 
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
			for (int i = 0; i < 3; ++i)
			{
				if (players[i])
					players[i]->Render(hWnd, g, imgManager);
			}
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
	switch (Msg)
	{
	case WM_LBUTTONDOWN:
	{
		// 내 인덱스 계산
		int myIdx = g_MyPlayerIndex;
		if (myIdx < 0 || myIdx >= 3)
			myIdx = 0;
		if (!players[myIdx] || !bullet)
			break;
		int mouseX = GET_X_LPARAM(lParam);
		int mouseY = GET_Y_LPARAM(lParam);
		// 플레이어 위치
		Gdiplus::PointF playerPos = players[myIdx]->GetPosition();
		// 방향벡터
		float dx = static_cast<float>(mouseX) - playerPos.X;
		float dy = static_cast<float>(mouseY) - playerPos.Y;

		float len = std::sqrt(dx * dx + dy * dy);
		if (len > 0.0f)
		{
			dx /= len;
			dy /= len;
		}

		// 총알 초기화
		bullet->Init(playerPos.X, playerPos.Y, dx, dy, 1);
		break;
	}

	case WM_RBUTTONDOWN:
	{
		// 내 인덱스 계산
		int myIdx = g_MyPlayerIndex;
		if (myIdx < 0 || myIdx >= 3)
			myIdx = 0;

		// 플레이어/수류탄이 준비되어 있을 때만 처리
		if (!players[myIdx] || !grenade)
			return;

		// 마우스 클릭 지점 (클라이언트 좌표계)
		int mouseX = static_cast<short>(LOWORD(lParam));
		int mouseY = static_cast<short>(HIWORD(lParam));

		Gdiplus::PointF startPos = players[myIdx]->GetPos();                 // 플레이어 중심
		Gdiplus::PointF targetPos(
			static_cast<float>(mouseX),
			static_cast<float>(mouseY)
		);

		// 기존 활성 상태 저장
		bool wasActive = grenade->IsActive();

		// 수류탄 투척 시도 (내부에서 2발 제한 체크)
		grenade->Throw(startPos, targetPos);

		// 이번 클릭에서 새로 활성화된 경우에만 서버에 알림
		if (!wasActive && grenade->IsActive() && g_NetworkRunning && g_ClientSock != INVALID_SOCKET)
		{
			// 플레이어 기준 마우스 방향 (라디안)
			float dirRad = player->CalculateAtan2MouseAtPos(hWnd);

			g_GrenadeReq.requested = true;
			g_GrenadeReq.dirRadAngle = dirRad;
		}

		break;
	}

	default:
		break;
	}
}
