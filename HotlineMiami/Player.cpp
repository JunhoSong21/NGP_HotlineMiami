#include "pch.h"
#include "Player.h"

Player::Player(POINT initPosition) :
	hWnd(nullptr),
	hBitmap(nullptr),
	playerPosition(initPosition),
	playerMoveState("Idle"),
	playerSpriteFrameNum(0)
{
}

Player::~Player()
{
}

bool Player::Init()
{
	hBitmap = (HBITMAP)LoadImageW(NULL, L"Jacket.bmp",
		IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	return true;
}

void Player::Update(float deltaTime)
{
	
}

void Player::Render(HWND hWnd, HDC hDC)
{
	int scaleWidth = 64;
	int scaleHeight = 64;

	HDC originDC = CreateCompatibleDC(hDC);
	HBITMAP oldOriginBitmap = (HBITMAP)SelectObject(originDC, hBitmap);

	HDC scaleDC = CreateCompatibleDC(hDC);
	HBITMAP scaleBitmap = CreateCompatibleBitmap(hDC, scaleWidth, scaleHeight);
	HBITMAP oldScaleBitmap = (HBITMAP)SelectObject(scaleDC, scaleBitmap);

	BITMAP bm;
	GetObject(hBitmap, sizeof(BITMAP), &bm);
	StretchBlt(scaleDC, 0, 0, scaleWidth, scaleHeight,
		originDC, 0, 0, 32, bm.bmHeight, SRCCOPY);

	SelectObject(originDC, oldOriginBitmap);
	DeleteDC(originDC);
	//////////////////////////////////////////////////////////
	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(hWnd, &mousePos);

	float radianAngle = atan2f(static_cast<float>(mousePos.y - playerPosition.y),
							static_cast<float>(mousePos.x - playerPosition.x));
	
	XFORM xForm;
	XFORM oldXForm;

	GetWorldTransform(hDC, &oldXForm);
	SetStretchBltMode(hDC, COLORONCOLOR);
	int oldGraphicMode = SetGraphicsMode(hDC, GM_ADVANCED);

	float centerX = static_cast<float>(scaleWidth) / 2.0f;
	float centerY = static_cast<float>(scaleHeight) / 2.0f;
	float newCenterX = static_cast<float>(playerPosition.x);
	float newCenterY = static_cast<float>(playerPosition.y);

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

void Player::InputProcessing(WPARAM wParam)
{
	float moveX = 0.0f;
	float moveY = 0.0f;

	// 각 키 상태 확인 및 이동 벡터 업데이트
	if (GetAsyncKeyState(VK_UP) & 0x8000) moveY -= 1.0f;
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) moveY += 1.0f;
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) moveX -= 1.0f;
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) moveX += 1.0f;

	// 대각선 이동 시 속도 정규화 (선택사항)
	if (moveX != 0.0f && moveY != 0.0f) {
		float length = sqrt(moveX * moveX + moveY * moveY);
		moveX /= length;
		moveY /= length;
	}

	// 최종 이동 적용 (deltaTime 활용)
	//playerPosition.x += moveX * deltaTime;
	//playerPosition.y += moveY * deltaTime;
}