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
    traveled(0.0f),
    maxCount(2),           // 최대 2발
    remainCount(2),         // 시작 시 2개 소지
    isInFuse(false),
    fuseDuration(4.0f),
    fuseRemain(0.0f)
{
}

void Grenade::Init()
{
    isActive = false;      
    traveled = 0.0f;
    maxDistance = 0.0f;
    remainCount = maxCount;  // 수류탄 2개로 리셋
    isInFuse = false;
    fuseDuration = 4.0f;
    fuseRemain = 0.0f;
}

void Grenade::LoadGrenadeImage(ImageManager& imgMgr)
{
    imgMgr.LoadSpriteImage(L"Resource/Grenade/sampleGrenade.png", spriteKey);
}

void Grenade::Update(float deltaTime)
{
    if (!isActive)
        return;

    if (deltaTime <= 0.0f)
        return;

    // 아직 비행 중인 상태라면 이동 처리
    if (!isInFuse)
    {
        float step = speed * deltaTime;
        // 남은 거리
        float remain = maxDistance - traveled;
        if (remain <= 0.0f) {
            // 도착 지점에 이미 도달 → 여기서 "폭발" 예정
            speed = 0.0f;
            isInFuse = true;
            fuseRemain = fuseDuration;   // 4초 카운트다운 시작
            return;
        }

        // 보정
        if (step > remain)
            step = remain;

        pos.X += dir.X * step;
        pos.Y += dir.Y * step;
        traveled += step;

        // 목표 지점까지 다 왔으면 여기서 멈추고 대기
        if (traveled >= maxDistance) {
            speed = 0.0f;
            isInFuse = true;
            fuseRemain = fuseDuration;
            return;
        }
    }
    // 폭발 시작
    Active(deltaTime);
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

    // 수류탄 실제 크기(스프라이트 기준)
    const float renderW = 18.0f;
    const float renderH = 15.0f;
    
    // pos를 중심으로 그리기
    Gdiplus::RectF dst(
        pos.X - renderW * 0.5f,
        pos.Y - renderH * 0.5f,
        renderW,
        renderH
    );

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

// 마우스 우클릭 지점까지 던지기 
void Grenade::Throw(const Gdiplus::PointF& startPos, const Gdiplus::PointF& targetPos)
{
    // 남은 수류탄이 없으면 발사 불가
    if (remainCount <= 0) {
        DEBUG_MSG(L"[Grenade] 남은 수류탄이 없어 더 이상 던질 수 없습니다.");
        return;
    }

    // 이미 공중에 날아가는 수류탄이 있으면 무시 (연속으로 두 발 동시에 X)
    if (isActive) {
        DEBUG_MSG(L"[Grenade] 기존 수류탄이 아직 날아가는 중입니다.");
        return;
    }

    // 방향 벡터 계산
    Gdiplus::PointF diff(
        targetPos.X - startPos.X,
        targetPos.Y - startPos.Y
    );

    float len = std::sqrt(diff.X * diff.X + diff.Y * diff.Y);
    if (len <= 0.0f) {
        // 제자리 클릭 같은 경우는 무시
        DEBUG_MSG(L"[Grenade] 목표 지점까지의 거리가 0입니다.");
        return;
    }

    dir.X = diff.X / len;
    dir.Y = diff.Y / len;

    pos = startPos;  // 시작 위치 = 플레이어 중심
    maxDistance = len;       // 목표 지점까지 거리
    traveled = 0.0f;
    isActive = true;

    // 퓨즈 상태 초기화
    isInFuse = false;
    fuseRemain = 0.0f;
    speed = 400.0f; // 다시 정상 속도로 세팅

    --remainCount;           // 한 발 소모
    DEBUG_MSG(L"[Grenade] 수류탄 투척! 남은 수류탄: %d", remainCount);
}

void Grenade::Active(float deltaTime)
{
    if (!isActive || !isInFuse)
        return;

    if (deltaTime <= 0.0f)
        return;

    fuseRemain -= deltaTime;
    if (fuseRemain <= 0.0f)
    {
        fuseRemain = 0.0f;
        // 지금은 단순히 사라지는 것까지만
        isActive = false;
        isInFuse = false;
        DEBUG_MSG(L"[Grenade] 폭발 시간 종료, 수류탄 비활성화");
    }
}

void Grenade::Reset()
{
    isActive = false;
    traveled = 0.0f;
    maxDistance = 0.0f;
    dir = Gdiplus::PointF(0.0f, 0.0f);
}
