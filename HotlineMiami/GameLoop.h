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

	Grenade* grenade;

	Bullet* bullet;

	float deltaTime;

	HWND hWnd;
	Camera* camera;
	bool isTitle;
	

public:
	GameLoop();
	~GameLoop();

	void Init(HWND hwnd);
	void Update();
	void Render();
	void InputProcessing(UINT Msg, WPARAM wParam, LPARAM lParam);
	bool IsTitle() const { return isTitle; }

	Bullet* GetBullet() { return bullet; }
	Player** GetPlayers() { return players; }
};