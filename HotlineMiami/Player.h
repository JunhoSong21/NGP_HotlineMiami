#pragma once
#include <cmath>
#include <numbers>
#include <string>
#include <map>

#include "ImageManager.h"

class Player {
private:
	std::wstring playerState;
	Gdiplus::PointF playerPos;

	int			playerSpriteFrameNum;
	int			spriteOriginWidth;
	int			spriteOriginHeight;
	int			spriteScaleMag;
	float		vectorX;
	float		vectorY;
	float		playerSpeed;
	float		frameTimeAccumulate;
	float 		radianAngle;
	HWND		hWnd;
	HBITMAP		hBitmap;

public:
	Player();
	~Player();
	
	Gdiplus::PointF& GetPos();
	bool Init();
	void Update(float deltaTime);
	void Render(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager);
	void InputProcessing(float deltaTime);
	
	// 플레이어 위치읽기 (Send_Input 에서 사용)
	const Gdiplus::PointF& GetPosition() const { return playerPos; }

	// 플레이어 포지션 변경 (RecvProcess 에서 사용)
	void SetPosition(float x, float y, float rad);

	void LoadPlayerImages(ImageManager& imgManager);
	void SpriteDivideAndRotateRender(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager);
	float CalculateAtan2MouseAtPos(HWND hWnd);
};