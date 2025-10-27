#pragma once
#include <Windows.h>

#include "BackGround.h"
#include "Timer.h"
#include "Player.h"

class GameLoop {
private:
	HWND hWnd;

	BackGround* backGround;
	Timer* timer;
	Player* player;
	float deltaTime;
	
public:
	GameLoop();
	~GameLoop();

	void Init(HWND hwnd);
	void Update();
	void Render();
	void InputProcessing(UINT Msg, WPARAM wParam, LPARAM lParam);
};