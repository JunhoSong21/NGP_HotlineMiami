#pragma once
#include <Windows.h>

#include "Timer.h"
#include "Player.h"

class GameLoop {
private:
	HWND hWnd;

	Timer* timer;
	Player* player;
	
public:
	GameLoop();
	~GameLoop();

	void Init(HWND hwnd);
	void Update();
	void Render();
	void InputProcessing(UINT iMsg, WPARAM wParam, LPARAM lParam);
};