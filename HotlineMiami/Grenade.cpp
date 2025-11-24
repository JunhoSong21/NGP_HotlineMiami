// Grenade.cpp
#include "pch.h"
#include "Grenade.h"
#include "Wall.h"

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
    fuseDuration(3.0f),
    fuseRemain(0.0f),
    wall(nullptr)
{
}

void Grenade::Init()
{
    isActive = false;      
    traveled = 0.0f;
    maxDistance = 0.0f;
    remainCount = maxCount;  // 수류탄 2개로 리셋
    isInFuse = false;
    fuseDuration = 3.0f;
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

        Gdiplus::PointF prevPos = pos;

        if (wall && Bounce(prevPos, step))
        {
            Active(deltaTime);
            return;
        }

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

    // 남은 시간 표시
    if (isInFuse)
    {
        // 남은 시간(초)을 0.0 단위로 표시
        wchar_t buf[32]{};
        swprintf_s(buf, L"%.1f", fuseRemain);

        Gdiplus::FontFamily fontFamily(L"Arial");
        Gdiplus::Font font(&fontFamily, 12.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPoint);
        Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0)); // 불투명 빨강
        Gdiplus::StringFormat format;
        format.SetAlignment(Gdiplus::StringAlignmentCenter);

        // 수류탄 바로 위에 텍스트 찍기
        Gdiplus::PointF textPos(
            pos.X,
            pos.Y - renderH * 0.5f - 15.0f
        );

        graphics.DrawString(
            buf,
            -1,
            &font,
            textPos,
            &format,
            &brush
        );
    }
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

bool Grenade::Bounce(const Gdiplus::PointF& prevPos, float step)
{
    if (!wall)
        return false;

    Gdiplus::PointF hitPoint;
    POINT           hitCell{ -1, -1 };

    // Raycast: prevPos → dir 방향으로 step 거리까지 검사
    if (!wall->Raycast(prevPos, dir, step, &hitPoint, &hitCell))
        return false;

    // 충돌한 타일의 월드 좌표 Rect
    Gdiplus::RectF tileRect = Wall::GridToWorldRect(hitCell.x, hitCell.y);

    float left = tileRect.X;
    float right = tileRect.GetRight();
    float top = tileRect.Y;
    float bottom = tileRect.GetBottom();

    float dl = std::fabs(hitPoint.X - left);
    float dr = std::fabs(hitPoint.X - right);
    float dt = std::fabs(hitPoint.Y - top);
    float db = std::fabs(hitPoint.Y - bottom);

    // 어느 면에서 가장 가까운지 판단
    float minDist = dl;
    int   side = 0; // 0: left, 1: right, 2: top, 3: bottom

    if (dr < minDist) { minDist = dr; side = 1; }
    if (dt < minDist) { minDist = dt; side = 2; }
    if (db < minDist) { minDist = db; side = 3; }

    // 면에 따른 법선 벡터
    Gdiplus::PointF n(0.0f, 0.0f);
    switch (side)
    {
    case 0: n = Gdiplus::PointF(-1.0f, 0.0f); break; // 왼쪽 면 → 법선 (-1,0)
    case 1: n = Gdiplus::PointF(1.0f, 0.0f); break; // 오른쪽 면 → (1,0)
    case 2: n = Gdiplus::PointF(0.0f, -1.0f); break; // 위쪽 면 → (0,-1)
    case 3: n = Gdiplus::PointF(0.0f, 1.0f); break; // 아래쪽 면 → (0,1)
    }

    // 반사 벡터 
    float dot = dir.X * n.X + dir.Y * n.Y;
    Gdiplus::PointF reflect(
        dir.X - 2.0f * dot * n.X,
        dir.Y - 2.0f * dot * n.Y
    );

    // 정규화
    float len = std::sqrt(reflect.X * reflect.X + reflect.Y * reflect.Y);
    if (len > 0.0f)
    {
        reflect.X /= len;
        reflect.Y /= len;
    }

    // 수류탄을 충돌 지점에서 벽 밖으로 살짝 밀어내기
    const float separate = 60.0f;
    pos = hitPoint;
    pos.X += reflect.X * separate;
    pos.Y += reflect.Y * separate;

    // 새 방향으로 교체
    dir = reflect;

    // 속도 감소 (벽에 맞으면 크게 감속)
    speed *= 0.4f;
    if (speed < 50.0f)
        speed = 0.0f;

    // 여기서 "active" 상태로 전환 (터질 준비)
    isInFuse = true;
    fuseRemain = fuseDuration;

    DEBUG_MSG(L"[Grenade] 벽에 충돌 -> 반사 및 속도 감소, 퓨즈 시작");

    return true;
}


void Grenade::Reset()
{
    isActive = false;
    traveled = 0.0f;
    maxDistance = 0.0f;
    dir = Gdiplus::PointF(0.0f, 0.0f);
}
