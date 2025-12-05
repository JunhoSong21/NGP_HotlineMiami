#include "pch.h"
#include "BackGround.h"

BackGround::BackGround() :
	HSV{0.0f, 0.8f, 0.7f},	// 색상 값, 채도, 명도
	RGB{0.0f, 0.0f, 0.0f},
	colorChangeSpeed(25.0f)
{
}

BackGround::~BackGround()
{
}

void BackGround::Update(float deltaTime)
{
	HSV.H += colorChangeSpeed * deltaTime; 

	if (HSV.H >= 360.0f) {
		HSV.H -= 360.0f;
	}
	else if (HSV.H < 0.0f) {
		HSV.H += 360.0f;
	}
}

void BackGround::Render(HWND hWnd, Gdiplus::Graphics& memoryGraphics)
{
	COLORREF color = HSVtoRGB();
	Gdiplus::Color backGroundColor(255,
		GetRValue(color), GetGValue(color), GetBValue(color));

	memoryGraphics.Clear(backGroundColor);
}

COLORREF BackGround::HSVtoRGB()
{
	float c = HSV.V * HSV.S;
	float x = c * (1 - abs(fmod(HSV.H / 60.0f, 2) - 1));
	float m = HSV.V - c;

	if (HSV.H >= 0.f && HSV.H < 60.f) {
		RGB.R = c;	RGB.G = x;	RGB.B = 0;
	}
	else if (HSV.H >= 60.f && HSV.H < 120.f) {
		RGB.R = x;	RGB.G = c;	RGB.B = 0;
	}
	else if (HSV.H >= 120.f && HSV.H < 180.f) {
		RGB.R = 0;	RGB.G = c;	RGB.B = x;
	}
	else if (HSV.H >= 180.f && HSV.H < 240.f) {
		RGB.R = 0;	RGB.G = x;	RGB.B = c;
	}
	else if (HSV.H >= 240.f && HSV.H < 300.f) {
		RGB.R = x;	RGB.G = 0;	RGB.B = c;
	}
	else {
		RGB.R = c;	RGB.G = 0;	RGB.B = x;
	}

	BYTE R = static_cast<BYTE>((RGB.R + m) * 255);
	BYTE G = static_cast<BYTE>((RGB.G + m) * 255);
	BYTE B = static_cast<BYTE>((RGB.B + m) * 255);

	return RGB(R, G, B);
}
