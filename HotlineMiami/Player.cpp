#include "pch.h"
#include "Player.h"

#include <atlimage.h>

Player::Player() :
	hWnd(nullptr),
	hBitmap(nullptr),
	playerPos{0.0f, 0.0f},
	playerState("Idle"),
	currentImage(nullptr),
	playerSpriteFrameNum(0),
	spriteOriginWidth(32),
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
	hBitmap = (HBITMAP)LoadImageW(NULL, L"Resource\\Sprite\\Jacket.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
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
	playerImages["Idle"].Load(L"Resource\\Sprite\\JacketWalk.png");

	currentImage = &playerImages.find("Idle")->second;
}

void Player::SpriteDivideAndRotateRender(HWND hWnd, HDC hDC)
{
	/*int scaleWidth = spriteOriginWidth * spriteScaleMag;
	int scaleHeight = spriteOriginHeight * spriteScaleMag;

	HDC scaleDC = CreateCompatibleDC(hDC);
	HBITMAP scaleBitmap = CreateCompatibleBitmap(hDC, scaleWidth, scaleHeight);
	HBITMAP oldScaleBitmap = (HBITMAP)SelectObject(scaleDC, scaleBitmap);

	
	BitBlt(scaleDC, 0, 0, scaleWidth, scaleHeight, NULL, 0, 0, BLACKNESS);
	currentImage->AlphaBlend(scaleDC, 0, 0, scaleWidth, scaleHeight,
							frameNum, 0, spriteOriginWidth, spriteOriginHeight);
	
	float radianAngle = CalculateAtan2MouseAtPos(hWnd, playerPos);

	XFORM xForm;
	XFORM oldXForm;

	GetWorldTransform(hDC, &oldXForm);
	SetStretchBltMode(hDC, COLORONCOLOR);
	int oldGraphicMode = SetGraphicsMode(hDC, GM_ADVANCED);

	float centerX = static_cast<float>(scaleWidth) / 2.0f;
	float centerY = static_cast<float>(scaleHeight) / 2.0f;

	ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);
	xForm = { 1.0f, 0.0f,
			0.0f, 1.0f,
			playerPos.x, playerPos.y };
	ModifyWorldTransform(hDC, &xForm, MWT_LEFTMULTIPLY);

	xForm = { cosf(radianAngle), sinf(radianAngle),
			-sinf(radianAngle), cosf(radianAngle),
			0.0f, 0.0f };
	ModifyWorldTransform(hDC, &xForm, MWT_LEFTMULTIPLY);

	xForm = { 1.0f, 0.0f,
			0.0f, 1.0f,
			-centerX, -centerY };
	ModifyWorldTransform(hDC, &xForm, MWT_LEFTMULTIPLY);

	StretchBlt(hDC, 0, 0, scaleWidth, scaleHeight,
		scaleDC, 0, 0, scaleWidth, scaleHeight, SRCCOPY);

	SetWorldTransform(hDC, &oldXForm);
	SetGraphicsMode(hDC, oldGraphicMode);

	SelectObject(scaleDC, oldScaleBitmap);
	DeleteObject(scaleBitmap);
	DeleteDC(scaleDC);*/

	HBITMAP hBitmapFromCImage = (HBITMAP)currentImage;
	Gdiplus::Bitmap* GdiplusBitmapPtr = Gdiplus::Bitmap::FromHBITMAP(hBitmapFromCImage, NULL);

	Gdiplus::Graphics graphics(hDC);
	graphics.SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
	graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

	Gdiplus::Matrix originMatrix;
	graphics.GetTransform(&originMatrix);

	float radianAngle = CalculateAtan2MouseAtPos(hWnd, playerPos);
	float degreeAngle = radianAngle * 180.0f / 3.14;
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