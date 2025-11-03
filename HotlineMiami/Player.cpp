#include "pch.h"
#include "Player.h"

Player::Player() :
	hWnd(nullptr),
	hBitmap(nullptr),
	playerPos{0.0f, 0.0f},
	playerState("Idle"),
	currentImage(nullptr),
	playerSpriteFrameNum(0),
	spriteOriginWidth(50),
	spriteOriginHeight(32),
	spriteScaleMag(2),
	vectorX(0.0f),
	vectorY(0.0f),
	playerSpeed(100.0f),
	frameTimeAccumulate(0.0f)
{
}

Player::~Player()
{
}

bool Player::Init()
{
	LoadPlayerImages();

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

void Player::Render(HWND hWnd, HDC hDC)
{
	SpriteDivideAndRotateRender(hWnd, hDC);
}

void Player::InputProcessing(float deltaTime)
{
	vectorX = 0.0f;
	vectorY = 0.0f;

	if (GetAsyncKeyState('W') & 0x8000) vectorY -= 1.0f;
	if (GetAsyncKeyState('S') & 0x8000) vectorY += 1.0f;
	if (GetAsyncKeyState('A') & 0x8000) vectorX -= 1.0f;
	if (GetAsyncKeyState('D') & 0x8000) vectorX += 1.0f;

	playerPos.x += vectorX * deltaTime * playerSpeed;
	playerPos.y += vectorY * deltaTime * playerSpeed;
}

void Player::LoadPlayerImages()
{
	HRESULT result;
	result = playerImages["Idle"].Load(L"Resource\\Sprite\\JacketWalk.png");
	if (FAILED(result))
		DEBUG_MSG(L"JacketWalk.png 파일 로드 실패");
	currentImage = &playerImages.find("Idle")->second;
}

void Player::SpriteDivideAndRotateRender(HWND hWnd, HDC hDC)
{
	if(!currentImage || currentImage->IsNull())
		DEBUG_MSG(L"currentImage nullptr")

	HBITMAP hBitmapFromCImage = (HBITMAP)*currentImage;
	Gdiplus::Bitmap* gdiplusBitmapPtr = Gdiplus::Bitmap::FromHBITMAP(hBitmapFromCImage, NULL);

	Gdiplus::Graphics graphics(hDC);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	Gdiplus::Matrix originMatrix;
	graphics.GetTransform(&originMatrix);

	float radianAngle = CalculateAtan2MouseAtPos(hWnd, playerPos);
	float degreeAngle = radianAngle * 180.0f / 3.1415;

	int scaleWidth = spriteOriginWidth * spriteScaleMag;
	int scaleHeight = spriteOriginHeight * spriteScaleMag;

	float centerX = static_cast<float>(scaleWidth) / 2.0f;
	float centerY = static_cast<float>(scaleHeight) / 2.0f;

	graphics.TranslateTransform(playerPos.x, playerPos.y);
	graphics.RotateTransform(degreeAngle);
	graphics.TranslateTransform(-centerX, -centerY);

	int frameNum = playerSpriteFrameNum * spriteOriginWidth;

	graphics.DrawImage(gdiplusBitmapPtr, 
		Gdiplus::RectF(0, 0, scaleWidth, scaleHeight),
		frameNum, 0, spriteOriginWidth, spriteOriginHeight,
		Gdiplus::UnitPixel);

	graphics.SetTransform(&originMatrix);
	delete gdiplusBitmapPtr;
}

float Player::CalculateAtan2MouseAtPos(HWND hWnd, Position playerPos)
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(hWnd, &mousePos);

	float radianAngle = atan2f(static_cast<float>(mousePos.y) - playerPos.y,
								static_cast<float>(mousePos.x) - playerPos.x);

	return radianAngle;
}