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

void BackGround::Update()
{
	HSV.H = GetCurrentHue();
}

void BackGround::Render(HWND hWnd, HDC hDC)
{
	RECT rect;
	GetClientRect(hWnd, &rect);

	COLORREF color = HSVtoRGB();
	HBRUSH brush = CreateSolidBrush(color);

	FillRect(hDC, &rect, brush);

	DeleteObject(brush);
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

float BackGround::GetCurrentHue()
{
	unsigned long long currenTime = GetTickCount64();

	return fmod(currenTime * colorChangeSpeed / 1000.0f, 360.0f);
}