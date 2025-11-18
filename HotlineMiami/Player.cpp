#include "pch.h"
#include "Player.h"

Player::Player() :
	playerState(L"PLAYER_IDLE"),
	playerPos{0.0f, 0.0f},
	playerSpriteFrameNum(0),
	spriteOriginWidth(50),
	spriteOriginHeight(32),
	spriteScaleMag(2),
	vectorX(0.0f),
	vectorY(0.0f),
	playerSpeed(100.0f),
	frameTimeAccumulate(0.0f),
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

void Player::Render(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager)
{
	SpriteDivideAndRotateRender(hWnd, graphics, imgManager);
}

void Player::InputProcessing(float deltaTime)
{
	vectorX = 0.0f;
	vectorY = 0.0f;

	if (GetAsyncKeyState('W') & 0x8000) vectorY -= 1.0f;
	if (GetAsyncKeyState('S') & 0x8000) vectorY += 1.0f;
	if (GetAsyncKeyState('A') & 0x8000) vectorX -= 1.0f;
	if (GetAsyncKeyState('D') & 0x8000) vectorX += 1.0f;

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