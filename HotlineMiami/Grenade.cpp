// Grenade.cpp
#include "pch.h"
#include "Grenade.h"

Grenade::Grenade() :
    spriteKey(L"GRENADE"),
    pos{ 0.0f, 0.0f },
    dir{ 0.0f, 0.0f },
    speed(400.0f),         // 필요하면 나중에 조절
    isActive(true),
    maxDistance(0.0f),
    traveled(0.0f)
{
}

void Grenade::Init()
{
    isActive = true;      // 테스트용
    traveled = 0.0f;
    maxDistance = 0.0f;
}

void Grenade::LoadGrenadeImage(ImageManager& imgMgr)
{
    imgMgr.LoadSpriteImage(L"Resource/Grenade/sampleGrenade.png", spriteKey);
}

void Grenade::Update(float deltaTime)
{
    if (!isActive)
        return;

    float step = speed * deltaTime;
    pos.X += dir.X * step;
    pos.Y += dir.Y * step;
    traveled += step;

    // 목표 지점 도달 or 살짝 넘어가면 정지 (폭발은 나중에 구현)
    if (traveled >= maxDistance) {
        // 너무 많이 간 경우 약간 되돌려서 정확히 끝점에 맞추기
        float overshoot = traveled - maxDistance;
        if (overshoot > 0.0f) {
            pos.X -= dir.X * overshoot;
            pos.Y -= dir.Y * overshoot;
        }

        // 기본 구현 단계에서는 그냥 비활성화(없애기)
        isActive = false;
    }
}

void Grenade::Render(Gdiplus::Graphics& graphics, ImageManager& imgMgr)
{
    if (!isActive)
        return;

    Gdiplus::Bitmap* bmp = imgMgr.GetImage(spriteKey);
    if (!bmp) {
        DEBUG_MSG(L"[Grenade] GetImage 실패 -> 키가 잘못 되었거나 이미지가 로드되지 않음");
        return;
    }

    // 화면에 찍을 위치 (테스트용 100,100, 32x32)
    const float drawX = 100.0f;
    const float drawY = 100.0f;
    const float renderW = 18.0f;
    const float renderH = 15.0f;

    Gdiplus::RectF dst(drawX, drawY, renderW, renderH);

    const float srcX = 0.0f;   
    const float srcY = 0.0f;   
    const float srcW = 18.0f;
    const float srcH = 15.0f;

    graphics.DrawImage(
        bmp,
        dst,
        srcX, srcY,
        srcW, srcH,
        Gdiplus::UnitPixel
    );
}

void Grenade::Reset()
{
    isActive = false;
    traveled = 0.0f;
    maxDistance = 0.0f;
    dir = Gdiplus::PointF(0.0f, 0.0f);
}
