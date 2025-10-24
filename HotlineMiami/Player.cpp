#include "pch.h"
#include "Player.h"

Player::Player(POINT initPos) :
	hWnd(nullptr),
	hBitmap(nullptr),
	playerPos(initPos),
	playerMoveState("Idle"),
	playerSpriteFrameNum(0),
	spriteOriginWidth(32),
	spriteOriginHeight(32),
	spriteScaleMag(2),
	vectorX(0.0f),
	vectorY(0.0f),
	playerSpeed(1.0f),
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

	playerPos.x += vectorX * deltaTime * playerSpeed;
	playerPos.y += vectorY * deltaTime * playerSpeed;
}

void Player::Render(HWND hWnd, HDC hDC)
{
	spriteDivideAndRotateRender(hWnd, hDC);
}

void Player::InputProcessing()
{
	vectorX = 0.0f;
	vectorY = 0.0f;

	if (GetAsyncKeyState('W') & 0x8000) vectorY -= 1.0f;
	if (GetAsyncKeyState('S') & 0x8000) vectorY += 1.0f;
	if (GetAsyncKeyState('A') & 0x8000) vectorX -= 1.0f;
	if (GetAsyncKeyState('D') & 0x8000) vectorX += 1.0f;
}

void Player::spriteDivideAndRotateRender(HWND hWnd, HDC hDC)
{
	int scaleWidth = spriteOriginWidth * spriteScaleMag;
	int scaleHeight = spriteOriginHeight * spriteScaleMag;

	HDC originDC = CreateCompatibleDC(hDC);
	HBITMAP oldOriginBitmap = (HBITMAP)SelectObject(originDC, hBitmap);

	HDC scaleDC = CreateCompatibleDC(hDC);
	HBITMAP scaleBitmap = CreateCompatibleBitmap(hDC, scaleWidth, scaleHeight);
	HBITMAP oldScaleBitmap = (HBITMAP)SelectObject(scaleDC, scaleBitmap);

	BITMAP bm;
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	int frameNum = playerSpriteFrameNum * spriteOriginWidth;
	StretchBlt(scaleDC, 0, 0, scaleWidth, scaleHeight,
		originDC, frameNum, 0, spriteOriginWidth, spriteOriginHeight, SRCCOPY);

	SelectObject(originDC, oldOriginBitmap);
	DeleteDC(originDC);
	//////////////////////////////////////////////////////////
	float radianAngle = CalculateAtan2MouseAtPos(hWnd, playerPos);

	XFORM xForm;
	XFORM oldXForm;

	GetWorldTransform(hDC, &oldXForm);
	SetStretchBltMode(hDC, COLORONCOLOR);
	int oldGraphicMode = SetGraphicsMode(hDC, GM_ADVANCED);

	float centerX = static_cast<float>(scaleWidth) / 2.0f;
	float centerY = static_cast<float>(scaleHeight) / 2.0f;
	float newCenterX = static_cast<float>(playerPos.x);
	float newCenterY = static_cast<float>(playerPos.y);

	ModifyWorldTransform(hDC, NULL, MWT_IDENTITY);
	xForm = { 1.0f, 0.0f,
			0.0f, 1.0f,
			newCenterX, newCenterY };
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
	DeleteDC(scaleDC);
}

float Player::CalculateAtan2MouseAtPos(HWND hWnd, POINT playerPos)
{
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(hWnd, &mousePos);

	float radianAngle = atan2f(static_cast<float>(mousePos.y - playerPos.y),
								static_cast<float>(mousePos.x - playerPos.x));

	return radianAngle;
}