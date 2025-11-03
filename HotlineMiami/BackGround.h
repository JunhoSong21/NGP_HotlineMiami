#pragma once
#include <cmath>

class BackGround {
private:
	struct ColorMapHSV {
		float H;
		float S;
		float V;
	};
	ColorMapHSV HSV;

	struct ColorMapRGB {
		float R;
		float G;
		float B;
	};
	ColorMapRGB RGB;

	float colorChangeSpeed;

public:
	BackGround();
	~BackGround();

	void Update();
	void Render(HWND hWnd, Gdiplus::Graphics& memoryGraphics);
	COLORREF HSVtoRGB();
	float GetCurrentHue();
};