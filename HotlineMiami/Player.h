#pragma once
#include <math.h>
#include <string>

class Player {
private:
	HWND		hWnd;
	HBITMAP		hBitmap;

	POINT		playerPos;
	std::string playerMoveState;
	int			playerSpriteFrameNum;
	int			spriteOriginWidth;
	int			spriteOriginHeight;
	int			spriteScaleMag;
	float		vectorX;
	float		vectorY;
	float		playerSpeed;
	float		frameTimeAccumulate;

public:
	Player(POINT initPos);
	~Player();

	bool Init();
	void Update(float deltaTime);
	void Render(HWND hWnd, HDC hDC);
	void InputProcessing();

	void spriteDivideAndRotateRender(HWND hWnd, HDC hDC);
	float CalculateAtan2MouseAtPos(HWND hWnd, POINT playerPos);
};