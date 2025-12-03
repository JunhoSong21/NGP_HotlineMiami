// Camera.cpp
#include "pch.h"
#include "Camera.h"

Camera::Camera() :
    position{ 0.0f, 0.0f },
    target{ 0.0f, 0.0f },
    viewWidth(1280.0f),
    viewHeight(800.0f)
{
}

void Camera::Update(float deltaTime)
{
    position = target;
}

void Camera::Apply(Gdiplus::Graphics& g) const
{
    // 카메라 중심을 화면 중앙
    float halfW = viewWidth * 0.5f;
    float halfH = viewHeight * 0.5f;

    g.TranslateTransform(halfW - position.X, halfH - position.Y);
}

Gdiplus::PointF Camera::ScreenToWorld(const POINT& screen) const
{
    float halfW = viewWidth * 0.5f;
    float halfH = viewHeight * 0.5f;

    // screen = world - position + (halfW, halfH)
    // world = screen - (halfW, halfH) + position
    return Gdiplus::PointF(
        static_cast<float>(screen.x) - halfW + position.X,
        static_cast<float>(screen.y) - halfH + position.Y
    );
}

Gdiplus::PointF Camera::WorldToScreen(const Gdiplus::PointF& world) const
{
    float halfW = viewWidth * 0.5f;
    float halfH = viewHeight * 0.5f;

    return Gdiplus::PointF(
        world.X - position.X + halfW,
        world.Y - position.Y + halfH
    );
}
