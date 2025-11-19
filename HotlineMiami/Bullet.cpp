#include "pch.h"
#include "Bullet.h"

Bullet::Bullet() :
    isVisible(false),
    x(0.0f),
    y(0.0f),
    dirX(0.0f),
    dirY(0.0f),
    speed(600.0f),
    ownerID(0),
    spriteWidth(16),
    spriteHeight(16),
    spriteScale(2)
{
}

Bullet::~Bullet()
{
}

void Bullet::Init(float startX, float startY, float dX, float dY, unsigned short owner)
{
    x = startX;
    y = startY;

    dirX = dX;
    dirY = dY;

    ownerID = owner;

    isVisible = true;
}

void Bullet::Update(float deltaTime)
{
    if (!isVisible) return;

    // 방향 벡터 * 속도 * deltaTime 만큼 이동
    x += dirX * speed * deltaTime;
    y += dirY * speed * deltaTime;

    // 화면 밖으로 나가면 비활성화
    if (x < -100.0f || x > 2000.0f || y < -100.0f || y > 2000.0f)
        isVisible = false;
}

void Bullet::Render(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager)
{
    if (!isVisible) return;

    SpriteRotateRender(hWnd, graphics, imgManager);
}

void Bullet::LoadBulletImages(ImageManager& imgManager)
{
    // Bullet 스프라이트 이미지 불러오는 함수
    // 2번째 인자로 key값을 넣고, 나중에 GetImage(L"BULLET")로 가져와서 사용
    imgManager.LoadSpriteImage(L"Resource/Sprite/Bullet.png", L"BULLET");
}

void Bullet::SpriteRotateRender(HWND hWnd, Gdiplus::Graphics& graphics, ImageManager& imgManager)
{
    Gdiplus::Bitmap* bulletImg = imgManager.GetImage(L"BULLET");
    if (!bulletImg) return;

    Gdiplus::Matrix originMatrix;
    graphics.GetTransform(&originMatrix);

    float radianAngle = CalculateDirectionAngle();
    float degreeAngle = radianAngle * 180.0f / PI;

    int drawW = spriteWidth * spriteScale;
    int drawH = spriteHeight * spriteScale;

    float centerX = static_cast<float>(drawW) / 2.0f;
    float centerY = static_cast<float>(drawH) / 2.0f;

    graphics.TranslateTransform(x, y);
    graphics.RotateTransform(degreeAngle);
    graphics.TranslateTransform(-centerX, -centerY);

    graphics.DrawImage(
        bulletImg,
        Gdiplus::RectF(0.0f, 0.0f, static_cast<float>(drawW), static_cast<float>(drawH))
    );

    graphics.SetTransform(&originMatrix);
}

float Bullet::CalculateDirectionAngle() const
{
    // dirX, dirY 방향벡터 기준으로 atan2
    float radianAngle = atan2f(dirY, dirX);
    return radianAngle;
}