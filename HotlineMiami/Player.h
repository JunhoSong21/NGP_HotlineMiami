#pragma once
#include <math.h>
#include <string>

class Player {
private:
	HWND		hWnd;
	HBITMAP		hBitmap;

	struct Position {
		float	x;
		float	y;
	};
	Position playerPos;
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
	Player();
	~Player();

	bool Init();
	void Update(float deltaTime);
	void Render(HWND hWnd, HDC hDC);
	void InputProcessing(float deltaTime);

	void spriteDivideAndRotateRender(HWND hWnd, HDC hDC);
	float CalculateAtan2MouseAtPos(HWND hWnd, Position playerPos);
};