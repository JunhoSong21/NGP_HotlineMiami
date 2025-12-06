#include "pch.h"
#include "Player.h"

Player::Player() :
	playerState(L"PLAYER_IDLE"),
	playerPos{300.0f, 500.0f},
	playerSpriteFrameNum(0),
	spriteOriginWidth(50),
	spriteOriginHeight(32),
	spriteScaleMag(2),
	vectorX(0.0f),
	vectorY(0.0f),
	playerSpeed(100.0f),
	frameTimeAccumulate(0.0f),
	hp(100.0f)
	, maxHp(100.0f),
	radianAngle(0.0f),
	hWnd(nullptr),
	hBitmap(nullptr),
	selectedNum(0)
{
}

Player::~Player()
{
}

bool Player::Init()
{
	return true;
}

int Player::GetDeathMaxFrames() const
{
	switch (selectedNum) {
	case 0: // Jacket
		return 4;
	case 1: // Alex
		return 11;
	case 2: // David
		return 11;
	default:
		return 4;
	}
}

// hp 바 확인용 데미지 함수
void Player::ApplyDamage(float amount)
{
	if (amount <= 0.0f) {
		return;
	}

	hp -= amount;
	if (hp < 0.0f) {
		if (playerState != L"PLAYER_DEATH") {
			SoundManager::Get().Play("death");
		}
		hp = 0.0f;
		// DEATH로 상태 전환
		playerState = L"PLAYER_DEATH";
		playerSpriteFrameNum = 0;
		frameTimeAccumulate = 0.0f;
	}
}

void Player::Update(float deltaTime, HWND hWnd, Camera* camera)
{
	if (playerState == L"PLAYER_DEATH") {
		// death 스프라이트는 4프레임
		// 약간 느리게 재생되도록 조정
		frameTimeAccumulate += deltaTime * 6.0f;	

		int maxFrames = GetDeathMaxFrames();

		if (frameTimeAccumulate >= 1.0f) {
			++playerSpriteFrameNum;
			if (playerSpriteFrameNum >= maxFrames) {
				playerSpriteFrameNum = maxFrames - 1;
			}
			frameTimeAccumulate -= 1.0f;
		}
		// 죽은 상태에서는 입력으로 인한 이동 없음
		return;
	}

	frameTimeAccumulate += deltaTime * 10.0f;
	if (frameTimeAccumulate >= 1.0f) {
		++playerSpriteFrameNum;
		if (8 == playerSpriteFrameNum)
			playerSpriteFrameNum = 0;

		frameTimeAccumulate -= 1.0f;
	}

	InputProcessing(deltaTime);

	radianAngle = CalculateAtan2MouseAtPos(hWnd, camera);
}

void Player::RenderHpBar(Gdiplus::Graphics& g)
{
	if (maxHp <= 0.0f) {
		return;
	}

	float ratio = hp / maxHp;
	if (ratio < 0.0f) {
		ratio = 0.0f;
	}
	if (ratio > 1.0f) {
		ratio = 1.0f;
	}

	// HP 바 크기
	const float barWidth = 40.0f; // 전체 길이
	const float barHeight = 5.0f;  // 높이
	const float offsetY = -35.0f; // 플레이어 머리 위로 조금 띄우기

	float barX = playerPos.X - barWidth * 0.5f;
	float barY = playerPos.Y + offsetY;

	// 바 배경 (살짝 반투명 검정)
	Gdiplus::RectF backRect(barX, barY, barWidth, barHeight);
	// 남은 체력 부분 (빨간색)
	Gdiplus::RectF hpRect(barX, barY, barWidth * ratio, barHeight);

	Gdiplus::SolidBrush backBrush(Gdiplus::Color(160, 0, 0, 0));      // 반투명 검정
	Gdiplus::SolidBrush hpBrush(Gdiplus::Color(255, 255, 0, 0));      // 빨간색

	g.FillRectangle(&backBrush, backRect);
	g.FillRectangle(&hpBrush, hpRect);
}

void Player::DebugRenderCollision(Gdiplus::Graphics& g)
{
	const float aabbWidth = 35.0f;
	const float aabbHeight = 35.0f;

	float left = playerPos.X - aabbWidth * 0.5f;
	float top = playerPos.Y - aabbHeight * 0.5f;

	Gdiplus::Pen pen(Gdiplus::Color(255, 255, 0, 0), 1.0f);
	g.DrawRectangle(&pen, left, top, aabbWidth, aabbHeight);
}

void Player::SpriteDeathRender(Gdiplus::Graphics& graphics, ImageManager& imgManager)
{
	std::wstring prefix;
	switch (selectedNum) {
	case 0: prefix = L"Jacket"; break;
	case 1: prefix = L"Alex";   break;
	case 2: prefix = L"David";  break;
	default: prefix = L"Jacket"; break;
	}

	std::wstring key = prefix + L"Death";

	Gdiplus::Bitmap* deathBitmap = imgManager.GetImage(key.c_str());
	if (!deathBitmap) {
		return;
	}

	int originWidth = 60;
	int originHeight = 60;
	int maxFrames = 4;

	switch (selectedNum) {
	case 0: // JacketDeath: 60x60, 4프레임 (기존)
		originWidth = 60;
		originHeight = 60;
		maxFrames = 4;
		break;

	case 1: // AlexDeath: 880x60, 11프레임 → 80x60
		originWidth = 80;
		originHeight = 60;
		maxFrames = 11;
		break;

	case 2: // DavidDeath: 770x40, 11프레임 → 70x40
		originWidth = 70;
		originHeight = 40;
		maxFrames = 11;
		break;
	}

	int frame = playerSpriteFrameNum;
	if (frame < 0) {
		frame = 0;
	}
	if (frame >= maxFrames) {
		frame = maxFrames - 1;
	}

	// 출력 크기: 일단 원본 기준으로 렌더 (나중에 필요하면 통일)
	float drawW = static_cast<float>(originWidth);
	float drawH = static_cast<float>(originHeight);

	float left = playerPos.X - drawW * 0.5f;
	float top = playerPos.Y - drawH * 0.5f;

	int srcX = frame * originWidth;
	int srcY = 0;

	graphics.DrawImage(
		deathBitmap,
		Gdiplus::RectF(left, top, drawW, drawH),       // 화면에 그릴 위치/크기
		static_cast<Gdiplus::REAL>(srcX),              // 스프라이트 시트에서 잘라올 X
		static_cast<Gdiplus::REAL>(srcY),              // 스프라이트 시트에서 잘라올 Y
		static_cast<Gdiplus::REAL>(originWidth),       // 잘라올 너비
		static_cast<Gdiplus::REAL>(originHeight),      // 잘라올 높이
		Gdiplus::UnitPixel
	);
}

void Player::Render(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager)
{
	if (playerState == L"PLAYER_DEATH") {
		// 죽은 상태
		SpriteDeathRender(graphics, imgManager);
	}
	else {
		// 살아있는 상태
		SpriteDivideAndRotateRender(hWnd, graphics, imgManager);
	}
	RenderHpBar(graphics);
}

void Player::InputProcessing(float deltaTime)
{
	// 사망 시 입력 방지
	if (IsDead()) {
		return;
	}
	
	vectorX = 0.0f;
	vectorY = 0.0f;

	if (GetAsyncKeyState('W') & 0x8000) vectorY -= 1.0f;
	if (GetAsyncKeyState('S') & 0x8000) vectorY += 1.0f;
	if (GetAsyncKeyState('A') & 0x8000) vectorX -= 1.0f;
	if (GetAsyncKeyState('D') & 0x8000) vectorX += 1.0f;

	//playerPos.X += vectorX * deltaTime * playerSpeed;
	//playerPos.Y += vectorY * deltaTime * playerSpeed;

	std::wstring newState;
	if (vectorX == 0.0f && vectorY == 0.0f) {
		newState = L"PLAYER_IDLE";
	}
	else {
		newState = L"PLAYER_WALK";
	}

	// 상태가 바뀔 때만 애니메이션 리셋 (프레임 튀는 것 방지)
	if (playerState != newState) {
		playerState = newState;
		playerSpriteFrameNum = 0;
		frameTimeAccumulate = 0.0f;
	}

	OutputDebugStringA("move!\n");
}

void Player::LoadPlayerImages(ImageManager& imgManager)
{
	// Player의 스프라이트 이미지 불러오는 함수
	// 2번째 인자로 key값을 넣어 해당 key와 playerState가 같으면 해당 스프라이트 사용

	// Jacket
	imgManager.LoadSpriteImage(L"Resource/Sprite/JacketIdle.png", L"JacketIdle");
	imgManager.LoadSpriteImage(L"Resource/Sprite/JacketWalk.png", L"JacketWalk");
	imgManager.LoadSpriteImage(L"Resource/Sprite/JacketDeath.png", L"JacketDeath");

	// Alex
	imgManager.LoadSpriteImage(L"Resource/Sprite/AlexIdle.png", L"AlexIdle");
	imgManager.LoadSpriteImage(L"Resource/Sprite/AlexWalk.png", L"AlexWalk");
	imgManager.LoadSpriteImage(L"Resource/Sprite/AlexDeath.png", L"AlexDeath");

	// David
	imgManager.LoadSpriteImage(L"Resource/Sprite/DavidIdle.png", L"DavidIdle");
	imgManager.LoadSpriteImage(L"Resource/Sprite/DavidWalk.png", L"DavidWalk");
	imgManager.LoadSpriteImage(L"Resource/Sprite/DavidDeath.png", L"DavidDeath");
}

void Player::SpriteDivideAndRotateRender(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager)
{
	std::wstring prefix;
	switch (selectedNum) {
	case 0: prefix = L"Jacket"; break;
	case 1: prefix = L"Alex";   break;
	case 2: prefix = L"David";  break;
	default: prefix = L"Jacket"; break;
	}

	std::wstring suffix;
	if (playerState == L"PLAYER_IDLE") {
		suffix = L"Idle";
	}
	else if (playerState == L"PLAYER_WALK") {
		suffix = L"Walk";
	}
	else {
		// DEATH는 별도 SpriteDeathRender에서 처리
		return;
	}

	std::wstring key = prefix + suffix;

	Gdiplus::Bitmap* currentBitmap = imgManager.GetImage(key.c_str());
	if (!currentBitmap) {
		return;
	}

	// 3) state + 스킨별 1프레임 원본 크기 설정
	int originWidth = 0;
	int originHeight = 0;

	if (playerState == L"PLAYER_IDLE") {
		originWidth = 32;
		originHeight = 32;
	}
	else if (playerState == L"PLAYER_WALK") {
		if (selectedNum == 0) {            // JacketWalk: 50x32 
			originWidth = 50;
			originHeight = 32;
		}
		else if (selectedNum == 1) {       // AlexWalk: 256x32 8프레임  32x32
			originWidth = 32;
			originHeight = 32;
		}
		else if (selectedNum == 2) {       // DavidWalk: 480x60 8프레임  60x60
			originWidth = 60;
			originHeight = 60;
		}
	}

	int scaleWidth = originWidth * spriteScaleMag;
	int scaleHeight = originHeight * spriteScaleMag;

	Gdiplus::Matrix originMatrix;
	graphics.GetTransform(&originMatrix);

	float degreeAngle = radianAngle * 180.0f / PI;

	float centerX = static_cast<float>(scaleWidth) / 3.0f;
	float centerY = static_cast<float>(scaleHeight) / 2.0f;

	graphics.TranslateTransform(playerPos.X, playerPos.Y);
	graphics.RotateTransform(degreeAngle);
	graphics.TranslateTransform(-centerX, -centerY);

	int frameNum = playerSpriteFrameNum * originWidth;

	graphics.DrawImage(
		currentBitmap,
		Gdiplus::RectF(0.0f, 0.0f,
			static_cast<Gdiplus::REAL>(scaleWidth),
			static_cast<Gdiplus::REAL>(scaleHeight)),
		static_cast<Gdiplus::REAL>(frameNum),
		0.0f,
		static_cast<Gdiplus::REAL>(originWidth),
		static_cast<Gdiplus::REAL>(originHeight),
		Gdiplus::UnitPixel
	);

	graphics.SetTransform(&originMatrix);
}

float Player::CalculateAtan2MouseAtPos(HWND hWnd, Camera* camera)
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(hWnd, &mousePos);

	Gdiplus::PointF worldMouse(
		static_cast<float>(mousePos.x),
		static_cast<float>(mousePos.y)
	);

	if (camera) {
		worldMouse = camera->ScreenToWorld(mousePos);
	}

	float radianAngle = atan2f(
		worldMouse.Y - playerPos.Y,
		worldMouse.X - playerPos.X
	);

	return radianAngle;
}

void Player::SetPosition(float x, float y, float rad)
{
	playerPos.X = x;
	playerPos.Y = y;
	radianAngle = rad;
}

Gdiplus::PointF& Player::GetPos()
{
	return playerPos;
}