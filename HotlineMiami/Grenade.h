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
    float             fuseRemain; // 서버에서 받은 값(0~3)
    bool              hasServerState; // 서버에서 상태 패킷을 한 번이라도 받았는지 여부

    struct Fragment {
        Gdiplus::PointF pos;
        Gdiplus::PointF vel;
        float life;   
        bool active;
    };
    std::vector<Fragment> fragments;
  
public:
    Grenade();
    ~Grenade() = default;

    void LoadGrenadeImage(ImageManager& imgMgr);
    // 서버 상태 적용
    void ApplyServerState(bool active, bool explode, float x, float y, float remainFuse);
    // 클라에서 폭발 VFX / 파편 애니메이션만 처리
    void Update(float deltaTime);
    void Render(Gdiplus::Graphics& graphics, ImageManager& imgMgr);
    // 폭발 시작 순간 한 번만 호출
    void SpawnFragments();
    bool IsActive() const { return isActive; }
    float GetRemainTime() const { return fuseRemain; }
    void SetPosition(float x, float y) { pos = Gdiplus::PointF(x, y); }
};
