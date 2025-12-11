#pragma once
#include <Windows.h>

#include "BackGround.h"
#include "Map.h"
#include "Wall.h"
#include "Timer.h"
#include "Player.h"

#include "Grenade.h"
#include "HUD.h"

#include "Bullet.h"


#include "ImageManager.h"
#include "SoundManager.h"
#include "Camera.h"

class GameLoop {
private:
	ImageManager imgManager;
	HUD* hud;

	BackGround* backGround;
	Map* map;
	Timer* timer;
	Player* players[3];
	Wall* wall;

	Grenade* grenades[3];

	Bullet* bullets[3];

	float deltaTime;

	HWND hWnd;
	Camera* camera;
	bool isTitle;

	Gdiplus::Bitmap* backBufferBitmap;  
	int backBufferWidth;
	int backBufferHeight;
	
public:
	GameLoop();
	~GameLoop();

	void Init(HWND hwnd);
	void Update();
	void Render();
	void InputProcessing(UINT Msg, WPARAM wParam, LPARAM lParam);
	bool IsTitle() const { return isTitle; }

	Player** GetPlayers() { return players; }
	Bullet** GetBullets() { return bullets; }
	Grenade** GetGrenades() { return grenades; }
};