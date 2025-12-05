#pragma once
#include "pch.h"

class Camera {
public:
    Camera();

    // 화면 크기 세팅 (Render에서 매 프레임 갱신해줄 예정)
    void SetViewSize(float w, float h) {
        viewWidth = w;
        viewHeight = h;
    }

    void SetTarget(const Gdiplus::PointF& targetPos) { target = targetPos;}
    void Update(float deltaTime);
    // Graphics에 카메라 변환 적용
    void Apply(Gdiplus::Graphics& g) const;

    // 좌표 변환 함수
    // - screen: 클라이언트 좌표
    // - return: 월드 좌표
    Gdiplus::PointF ScreenToWorld(const POINT& screen) const;
    Gdiplus::PointF WorldToScreen(const Gdiplus::PointF& world) const;

private:
    Gdiplus::PointF position;    // 카메라 중심 (world)
    Gdiplus::PointF target;      // 따라갈 목표 위치

    float viewWidth;
    float viewHeight;
};
