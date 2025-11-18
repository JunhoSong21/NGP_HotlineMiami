// Grenade.cpp
#include "pch.h"
#include "Grenade.h"

Grenade::Grenade() :
    spriteKey(L"GRENADE"),
    pos{ 0.0f, 0.0f },
    dir{ 0.0f, 0.0f },
    speed(400.0f),         // 필요하면 나중에 조절
    isActive(false),
    maxDistance(0.0f),
    traveled(0.0f)
{
}

void Grenade::Init(ImageManager& imgMgr)
{
    // 실제 파일 이름에 맞게 경로/이름만 수정하면 됨
    // 예) Resource/Grenade/sprGrenade.png
    imgMgr.LoadSpriteImage(L"Resource/Grenade/sprGrenade.png", spriteKey);
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
    if (!bmp)
        return;

    const float w = static_cast<float>(bmp->GetWidth());
    const float h = static_cast<float>(bmp->GetHeight());

    // 수류탄이 중심에 오도록 렌더
    Gdiplus::RectF dst(
        pos.X - w * 0.5f,
        pos.Y - h * 0.5f,
        w,
        h
    );

    graphics.DrawImage(
        bmp,
        dst,
        0, 0,
        static_cast<INT>(w),
        static_cast<INT>(h),
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
