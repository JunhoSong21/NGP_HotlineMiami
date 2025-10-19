#pragma once
#include <math.h>
#include <string>

class Player {
private:
	HWND hWnd;
	HBITMAP hBitmap;

	POINT playerPosition;
	std::string playerMoveState;

public:
	Player(POINT initPosition);
	~Player();

	bool Init();
	void Update(float deltaTime);
	void Render(HWND hWnd, HDC hDC);
	void InputProcessing(WPARAM wParam);
};