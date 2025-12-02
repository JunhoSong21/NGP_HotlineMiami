#pragma once
#include "pch.h"
#include "ImageManager.h"

class Bullet {
private:
    // 총알이 화면에서 사라졌는지 아닌지
    bool isVisible;
	// 총알의 현재 위치
    float x;
    float y;
	// 총알의 진행 방향 벡터
    float dirX;
    float dirY;
	// 총알의 속도
    float speed;
	// 총알 소유자 ID
    unsigned short ownerID;
	// 스프라이트 폭/높이/스케일
    int spriteWidth;
    int spriteHeight;
    int spriteScale;

public:
    Bullet();
    ~Bullet();

    void Init(float startX, float startY, float dX, float dY, unsigned short owner);
    void Update(float deltaTime);
    void Render(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager);
    
    // 총알 위치
    Gdiplus::PointF GetPosition() const { return Gdiplus::PointF(x, y); }
    
    void LoadBulletImages(ImageManager& imgManager);
    void SpriteRotateRender(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager);


    // 현재 총알의 방향을 반환 (dirX, dirY 기준)
    float CalculateDirectionAngle() const;

    // 총알을 다른 클래스가 안전하게 읽을 수 있도록 제공
    bool IsVisible() const { return isVisible; }
    void SetVisible(bool v) { isVisible = v; }

    float GetX() const { return x; }
    float GetY() const { return y; }

    void SetPosition(float nx, float ny) { x = nx; y = ny; }
    void SetDirection(float dx, float dy) { dirX = dx; dirY = dy; }
};