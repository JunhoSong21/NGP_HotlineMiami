#pragma once
#include "pch.h"
#include "ImageManager.h"
#include "SoundManager.h"
#include <vector>

class Player;
class Wall;

class Grenade {
private:
    std::wstring spriteKey;   // 이미지 키
    Gdiplus::PointF pos;      // 현재 위치
    bool isActive;            // 현재 활성(날아가는 중) 여부
    bool exploded;   // 폭발 여부 (서버 상태)
    std::wstring            fragmentSpriteKey; // 파편 이미지 키
   
    struct Fragment {
        Gdiplus::PointF pos;
        Gdiplus::PointF vel;
        float life;   
        bool active;
    };

    std::vector<Fragment> fragments;

    float fuseTime;     
    float maxFuseTime;   
public:
    Grenade();
    ~Grenade() = default;


    // 수류탄 개수 초기화
    void Init();
    // 리소스 로드용
    void LoadGrenadeImage(ImageManager& imgMgr);
    void Update(float deltaTime);
    void Render(Gdiplus::Graphics& graphics, ImageManager& imgMgr);
    void SpawnFragments();
    // 발사 (플레이어 위치, 목표 위치)
    void Throw(const Gdiplus::PointF& startPos, const Gdiplus::PointF& targetPos);
    bool IsActive() const { return isActive; }
    float GetRemainTime() const { return fuseTime; }
};
