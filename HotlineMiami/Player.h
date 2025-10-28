#pragma once
#include <math.h>
#include <string>
#include <atlimage.h>
#include <map>

class Player {
private:
	HWND		hWnd;
	HBITMAP		hBitmap;

	struct Position {
		float	x;
		float	y;
	};
	Position playerPos;

	std::string playerState;
	std::map<std::string, CImage> playerImages;
	CImage* currentImage;
	
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

	void LoadPlayerImages();
	void SpriteDivideAndRotateRender(HWND hWnd, HDC hDC);
	float CalculateAtan2MouseAtPos(HWND hWnd, Position playerPos);
};