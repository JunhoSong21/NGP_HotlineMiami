#pragma once
#include "pch.h"
#include "ImageManager.h"
#include <vector>

class Wall;

class Grenade {
private:
    std::wstring spriteKey;   // 이미지 키
    Gdiplus::PointF pos;      // 현재 위치
    Gdiplus::PointF dir;      // 정규화된 이동 방향
    float speed;              // 이동 속도 (px/sec)
    bool isActive;            // 현재 활성(날아가는 중) 여부

    float maxDistance;        // 목표 지점까지 거리
    float traveled;           // 지금까지 이동한 거리
    int            maxCount;     // 최대 소지 개수 (2)
    int            remainCount;  // 남은 수류탄 개수
    const Wall* wall;

    // 폭발 관련
    bool  isInFuse;      // true면 멈춰서 카운트다운 중
    float fuseDuration;  // 전체 대기 시간(초) 
    float fuseRemain;    // 남은 시간(초)     

    // 파편 구조체
    struct Fragment {
        Gdiplus::PointF pos;   // 파편 위치
        Gdiplus::PointF dir;   // 파편 방향
        float           speed; // 파편 속도
        float           life;  // 남은 수명(초)
        bool            active;
    };

    std::wstring            fragmentSpriteKey; // 파편 이미지 키
    std::vector<Fragment>   fragments;         // 현재 활성 파편들

public:
    Grenade();
    ~Grenade() = default;

    // 수류탄 개수 초기화
    void Init();
    // 리소스 로드용
    void LoadGrenadeImage(ImageManager& imgMgr);
    void Update(float deltaTime);
    void Render(Gdiplus::Graphics& graphics, ImageManager& imgMgr);
    // 발사 (플레이어 위치, 목표 위치)
    void Throw(const Gdiplus::PointF& startPos, const Gdiplus::PointF& targetPos);

    void Active(float deltaTime);
    void Explode();
    bool IsActive() const { return isActive; }
    int  GetRemain()    const { return remainCount; }

    void Reset();

    void SetWall(const Wall* w) { wall = w; }
    bool Bounce(const Gdiplus::PointF& prevPos, float step);
};
