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
	hWnd(nullptr),
	camera(nullptr),
	isTitle(true)

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

	if (backGround) { delete backGround; backGround = nullptr; }
	if (map) { delete map;        map = nullptr; }
	if (wall) { delete wall;       wall = nullptr; }
	if (timer) { delete timer;      timer = nullptr; }
	if (grenade) { delete grenade;    grenade = nullptr; }
	if (hud) { delete hud;        hud = nullptr; }
	if (bullet) { delete bullet;     bullet = nullptr; }
	if (camera) { delete camera;     camera = nullptr; }
}

void GameLoop::Init(HWND hwnd)
{
	hWnd = hwnd;
	
	imgManager.LoadSpriteImage(
		L"Resource/Start/StartImage.png",
		L"StartImage"               
	);
	// 사운드 로딩
	SoundManager::Get().LoadSound("bgm", "Resource/Sound/BGM.wav");
	SoundManager::Get().LoadSound("grenade_explosion", "Resource/Sound/GrenadeExplosion.wav");
	SoundManager::Get().LoadSound("death", "Resource/Sound/Death.wav");
	SoundManager::Get().PlayBGM("bgm");


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

	// 수류탄에 벽 정보 넘겨주기
	if (wall && grenade)
		grenade->SetWall(wall);

	hud = new HUD();
	hud->Init(imgManager);
	players[0]->LoadPlayerImages(imgManager);

	bullet = new Bullet();
	bullet->LoadBulletImages(imgManager);
	bullet->SetVisible(false);

	// 카메라 세팅
	if (not camera) {
		camera = new Camera();

		// 윈도우 클라이언트 크기 기준
		RECT rc{};
		GetClientRect(hWnd, &rc);
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;
		if (width <= 0) width = 1280;
		if (height <= 0) height = 800;

		camera->SetViewSize(static_cast<float>(width), static_cast<float>(height));

		// 시작 타겟: 내 플레이어 위치
		int myIdx = g_MyPlayerIndex;
		if (myIdx < 0 || myIdx >= 3) myIdx = 0;
		if (players[myIdx]) {
			camera->SetTarget(players[myIdx]->GetPos());
			camera->Update(0.0f);
		}
	}

	OutputDebugStringA("Init!\n");
}

void GameLoop::Update()
{
	SoundManager::Get().Update();   // 재생 끝난 SFX 보이스 정리

	if (backGround) backGround->Update();
	if (timer) deltaTime = timer->getDeltaTime();

	if (isTitle)
		return;

	// 내 인덱스 계산
	int myIdx = g_MyPlayerIndex;
	if (myIdx < 0 || myIdx >= 3)
		myIdx = 0;


	if (players[myIdx])
	{
		Gdiplus::PointF oldPos = players[myIdx]->GetPos();

		players[myIdx]->Update(deltaTime, hWnd, camera);

		Gdiplus::PointF newPos = players[myIdx]->GetPos();
		Gdiplus::PointF delta(newPos.X - oldPos.X,
			newPos.Y - oldPos.Y);

		Gdiplus::SizeF playerAabb(35.0f, 35.0f);
		Gdiplus::PointF resolvedPos = oldPos;

		if (wall)
			wall->ResolveMove(resolvedPos, playerAabb, delta);

		players[myIdx]->GetPos() = resolvedPos;
	}

	if (camera && players[myIdx]) {
		camera->SetTarget(players[myIdx]->GetPos());
		camera->Update(deltaTime);
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
		DEBUG_MSG(L"[GetDC Error] : 화면 HDC 획득 실패");

	RECT clientRect;
	GetClientRect(this->hWnd, &clientRect);
	int width = clientRect.right - clientRect.left;
	int height = clientRect.bottom - clientRect.top;
	if (width <= 0 || height <= 0)
		DEBUG_MSG(L"[ClientRect Error] : width 또는 height 값이 0 이하");

	if (camera) {
		camera->SetViewSize(static_cast<float>(width), static_cast<float>(height));
	}

	Gdiplus::Bitmap* backBufferBitmap = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);
	if (!backBufferBitmap || backBufferBitmap->GetLastStatus() != Gdiplus::Ok) {
		DEBUG_MSG(L"[BackBufferBitmap Error] : 백버퍼 Bitmap 생성 실패");
	}

	{
		Gdiplus::Graphics g(backBufferBitmap);
		g.Clear(Gdiplus::Color(0, 0, 0, 0));

		//title screen
		if (isTitle)
		{
			Gdiplus::Bitmap* titleBmp = imgManager.GetImage(L"StartImage");
			if (titleBmp)
			{
				g.DrawImage(titleBmp, 0, 0, width, height);
			}
			
			Gdiplus::Graphics screen(hDC);
			screen.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
			screen.DrawImage(backBufferBitmap, 0, 0, width, height);

			delete backBufferBitmap;
			ReleaseDC(hWnd, hDC);
			return;   
		}

		// 1) backGround
		{
			auto s = g.Save();
			g.SetCompositingMode(Gdiplus::CompositingModeSourceCopy);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			if (backGround) backGround->Render(hWnd, g); 
			g.Restore(s);
		}

		{
			auto sWorld = g.Save();

			if (camera) {
				camera->Apply(g);    // 여기서부터는 전부 월드 좌표 기준
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
		// 4) Grenade 
		if (grenade) grenade->Render(g, imgManager);
		
		// 5) player
		{
			auto s = g.Save();
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
			g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
			for (int i = 0; i < 3; ++i)
			{
				if (players[i]) {
					players[i]->Render(hWnd, g, imgManager);
#ifdef _DEBUG
					players[i]->DebugRenderCollision(g);	// 충돌 영역 렌더링
#endif

				}
			}
			g.Restore(s);
		}

		// 6) bullet
		{
			auto s = g.Save();
			g.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
			g.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);
			g.SetCompositingQuality(Gdiplus::CompositingQualityHighQuality);
			g.SetCompositingMode(Gdiplus::CompositingModeSourceOver);
			if (bullet) bullet->Render(hWnd, g, imgManager);
			g.Restore(s);
		}
		g.Restore(sWorld);
		}

		{
			auto s = g.Save();
			// 여기서는 Transform 적용 안 함 → 순수 화면 좌표 기준
			if (hud) hud->Render(hWnd, g, imgManager);
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
	// 내 인덱스 계산
	int myIdx = g_MyPlayerIndex;
	if (myIdx < 0 || myIdx >= 3)
		myIdx = 0;

	// 플레이어 사망 시 입력 방지
	if (players[myIdx]->IsDead()) {
		return;
	}

	switch (Msg)
	{
	case WM_LBUTTONDOWN:
	{
		if (isTitle)
		{
			RECT rc;
			GetClientRect(hWnd, &rc);
			int width = rc.right - rc.left;
			int height = rc.bottom - rc.top;

			float sx = width / 1920.0f;
			float sy = height / 1200.0f;

			int btnLeft = (int)(260 * sx);
			int btnRight = (int)(760 * sx);
			int btnTop = (int)(550 * sy);
			int btnBottom = (int)(840 * sy);

			int mouseX = GET_X_LPARAM(lParam);
			int mouseY = GET_Y_LPARAM(lParam);

			if (mouseX >= btnLeft && mouseX <= btnRight &&
				mouseY >= btnTop && mouseY <= btnBottom)
			{
				isTitle = false; 
			}

			return; 
		}

		if (!players[myIdx] || !bullet)
			break;
		int mouseX = GET_X_LPARAM(lParam);
		int mouseY = GET_Y_LPARAM(lParam);

		Gdiplus::PointF mouseWorld(static_cast<float>(mouseX), static_cast<float>(mouseY));
		if (camera) {
			POINT pt{ mouseX, mouseY };
			mouseWorld = camera->ScreenToWorld(pt);
		}

		// 플레이어 위치
		Gdiplus::PointF playerPos = players[myIdx]->GetPosition();
		// 방향벡터
		float dx = mouseWorld.X - playerPos.X;
		float dy = mouseWorld.Y - playerPos.Y;

		float len = std::sqrt(dx * dx + dy * dy);
		if (len > 0.0f)
		{
			dx /= len;
			dy /= len;
		}

		// 총알 초기화
		bullet->Init(playerPos.X, playerPos.Y, dx, dy, 1);

		float angle = atan2f(dy, dx);
		g_BulletReq.requested = true;
		g_BulletReq.dirRadAngle = angle;
		break;
	}

	case WM_RBUTTONDOWN:
	{
		

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

		if (camera) {
			POINT pt{ mouseX, mouseY };
			targetPos = camera->ScreenToWorld(pt);
		}

		// 기존 활성 상태 저장
		bool wasActive = grenade->IsActive();

		// 수류탄 투척 시도 (내부에서 2발 제한 체크)
		grenade->Throw(startPos, targetPos);

		// 이번 클릭에서 새로 활성화된 경우에만 서버에 알림
		if (!wasActive && grenade->IsActive() && g_NetworkRunning && g_ClientSock != INVALID_SOCKET)
		{
			float dx = targetPos.X - startPos.X;
			float dy = targetPos.Y - startPos.Y;
			float dirRad = atan2f(dy, dx);

			g_GrenadeReq.requested = true;
			g_GrenadeReq.dirRadAngle = dirRad;
		}

		break;
	}
	default:
		break;
	}
}
