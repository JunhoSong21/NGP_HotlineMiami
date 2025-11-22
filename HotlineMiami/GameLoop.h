#pragma once
#include <Windows.h>

#include "BackGround.h"
#include "Map.h"
#include "Wall.h"
#include "Timer.h"
#include "Player.h"
#include "Bullet.h"

#include "ImageManager.h"

class GameLoop {
private:
	ImageManager imgManager;

	BackGround* backGround;
	Map* map;
	Timer* timer;
	Player* player;
	Wall* wall;
	Bullet* bullet;
	float deltaTime;

	HWND hWnd;

	
public:
	GameLoop();
	~GameLoop();

	void Init(HWND hwnd);
	void Update();
	void Render();
	void InputProcessing(UINT Msg, WPARAM wParam, LPARAM lParam);
	Player* GetPlayer() { return player; } // Gammeloop에서 player 꺼낼 수 있도록
};