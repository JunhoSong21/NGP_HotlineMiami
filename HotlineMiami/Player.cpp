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
	hWnd(nullptr),
	hBitmap(nullptr)
{
}

Player::~Player()
{
}

bool Player::Init()
{
	return true;
}

// hp 바 확인용 데미지 함수
void Player::ApplyDamage(float amount)
{
	if (amount <= 0.0f) {
		return;
	}

	hp -= amount;
	if (hp < 0.0f) {
		hp = 0.0f;
	}

	// TODO: 나중에 여기서 hp <= 0일 때 사망 처리 넣으면 됨
}

void Player::Update(float deltaTime)
{
	frameTimeAccumulate += deltaTime * 10.0f;
	if (frameTimeAccumulate >= 1.0f) {
		++playerSpriteFrameNum;
		if (8 == playerSpriteFrameNum)
			playerSpriteFrameNum = 0;

		frameTimeAccumulate -= 1.0f;
	}

	InputProcessing(deltaTime);
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

	float barX = playerPos.X - barWidth * 0.5f - 15.0f;
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

void Player::Render(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager)
{
	SpriteDivideAndRotateRender(hWnd, graphics, imgManager);
	RenderHpBar(graphics);
}

void Player::InputProcessing(float deltaTime)
{
	vectorX = 0.0f;
	vectorY = 0.0f;

	if (GetAsyncKeyState('W') & 0x8000) vectorY -= 1.0f;
	if (GetAsyncKeyState('S') & 0x8000) vectorY += 1.0f;
	if (GetAsyncKeyState('A') & 0x8000) vectorX -= 1.0f;
	if (GetAsyncKeyState('D') & 0x8000) vectorX += 1.0f;
	OutputDebugStringA("move!\n");
	playerPos.X += vectorX * deltaTime * playerSpeed;
	playerPos.Y += vectorY * deltaTime * playerSpeed;
}

void Player::LoadPlayerImages(ImageManager& imgManager)
{
	// Player의 스프라이트 이미지 불러오는 함수
	// 2번째 인자로 key값을 넣어 해당 key와 playerState가 같으면 해당 스프라이트 사용
	imgManager.LoadSpriteImage(L"Resource/Sprite/JacketWalk.png", L"PLAYER_IDLE");
}

void Player::SpriteDivideAndRotateRender(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager)
{
	Gdiplus::Bitmap* currentBitmap = imgManager.GetImage(playerState);

	Gdiplus::Matrix originMatrix;
	graphics.GetTransform(&originMatrix);

	float radianAngle = CalculateAtan2MouseAtPos(hWnd);
	float degreeAngle = radianAngle * 180.0f / PI;

	int scaleWidth = spriteOriginWidth * spriteScaleMag;
	int scaleHeight = spriteOriginHeight * spriteScaleMag;

	float centerX = static_cast<float>(scaleWidth) / 2.0f;
	float centerY = static_cast<float>(scaleHeight) / 2.0f;

	graphics.TranslateTransform(playerPos.X, playerPos.Y);
	graphics.RotateTransform(degreeAngle);
	graphics.TranslateTransform(-centerX, -centerY);

	int frameNum = playerSpriteFrameNum * spriteOriginWidth;

	graphics.DrawImage(currentBitmap, 
		Gdiplus::RectF(0, 0, scaleWidth, scaleHeight),
		frameNum, 0, spriteOriginWidth, spriteOriginHeight,
		Gdiplus::UnitPixel);

	graphics.SetTransform(&originMatrix);
}

float Player::CalculateAtan2MouseAtPos(HWND hWnd)
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(hWnd, &mousePos);

	float radianAngle = atan2f(static_cast<float>(mousePos.y) - playerPos.Y,
								static_cast<float>(mousePos.x) - playerPos.X);

	return radianAngle;
}

Gdiplus::PointF& Player::GetPos()
{
	return playerPos;
}