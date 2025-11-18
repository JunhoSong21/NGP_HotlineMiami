#pragma once
#include "pch.h"
#include "ImageManager.h"

class Grenade {
private:
    std::wstring spriteKey;   // 이미지 키
    Gdiplus::PointF pos;      // 현재 위치
    Gdiplus::PointF dir;      // 정규화된 이동 방향
    float speed;              // 이동 속도 (px/sec)
    bool isActive;            // 현재 활성(날아가는 중) 여부

    float maxDistance;        // 목표 지점까지 거리
    float traveled;           // 지금까지 이동한 거리

public:
    Grenade();
    ~Grenade() = default;

    // 리소스 로드용
    void Init(ImageManager& imgMgr);
    void Update(float deltaTime);
    void Render(Gdiplus::Graphics& graphics, ImageManager& imgMgr);
    // 발사 (플레이어 위치, 목표 위치)
    void Throw(const Gdiplus::PointF& startPos, const Gdiplus::PointF& targetPos);

    bool IsActive() const { return isActive; }
    void Reset();
};
