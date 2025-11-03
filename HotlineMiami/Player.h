#pragma once
#include <math.h>
#include <string>
#include <map>

#include "ImageManager.h"

class Player {
private:
	std::wstring playerState;
	//std::map<std::string, CImage> playerImages;

	struct Position {
		float	x;
		float	y;
	};
	Position playerPos;

	int			playerSpriteFrameNum;
	int			spriteOriginWidth;
	int			spriteOriginHeight;
	int			spriteScaleMag;
	float		vectorX;
	float		vectorY;
	float		playerSpeed;
	float		frameTimeAccumulate;

	HWND		hWnd;
	HBITMAP		hBitmap;

public:
	Player();
	~Player();

	bool Init();
	void Update(float deltaTime);
	void Render(HWND hWnd, HDC hDC);
	void InputProcessing(float deltaTime);

	void LoadPlayerImages(ImageManager& imgManager);
	void SpriteDivideAndRotateRender(HWND hWnd, HDC hDC, ImageManager& imgManager);
	float CalculateAtan2MouseAtPos(HWND hWnd, Position playerPos);

	Gdiplus::PointF GetPlayerPos();
};