// Grenade.cpp
#include "pch.h"
#include "Grenade.h"
#include "Wall.h"
#include "Player.h" 

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
    wall(nullptr),
    fragmentSpriteKey(L"GRENADE_FRAG")
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
    fragments.clear();  // 파편 초기화
}

void Grenade::LoadGrenadeImage(ImageManager& imgMgr)
{
    // 수류탄 이미지 로드
    imgMgr.LoadSpriteImage(L"Resource/Grenade/sampleGrenade.png", spriteKey);

    // 파편 이미지 로드
    imgMgr.LoadSpriteImage(L"Resource/Grenade/sprGrenadeFragment.png", fragmentSpriteKey);
}

void Grenade::Update(float deltaTime)
{
    if (deltaTime <= 0.0f)
        return;

    if (isActive)
    {
        // 아직 퓨즈가 안 켜졌으면 계속 날아감
        if (!isInFuse)
        {
            float step = speed * deltaTime;

            // 속도가 0인데 아직 퓨즈 안 켜졌으면 퓨즈 시작
            if (step <= 0.0f)
            {
                isInFuse = true;
                fuseRemain = fuseDuration;
                Active(deltaTime);
            }
            else
            {
                float remainDist = maxDistance - traveled;
                if (remainDist <= 0.0f)
                {
                    // 목표 지점 도달 → 멈추고 퓨즈 시작
                    speed = 0.0f;
                    isInFuse = true;
                    fuseRemain = fuseDuration;
                    Active(deltaTime);
                }
                else
                {
                    if (step > remainDist)
                        step = remainDist;

                    Gdiplus::PointF prevPos = pos;

                    if (wall && Bounce(prevPos, step))
                    {
                        // 충돌 시 step 만큼 이동했다고 간주
                        traveled += step;
                    }
                    else
                    {
                        pos.X += dir.X * step;
                        pos.Y += dir.Y * step;
                        traveled += step;
                    }

                    const float fuseSpeedThreshold = 80.0f;
                    if (traveled >= maxDistance || speed <= fuseSpeedThreshold)
                    {
                        speed = 0.0f;
                        isInFuse = true;
                        fuseRemain = fuseDuration;
                    }
                }
            }
        }
        // 퓨즈 카운트다운 및 폭발 처리
        Active(deltaTime);
    }
    // 파편 업데이트
    if (!fragments.empty())
    {
        const float dt = deltaTime;

        for (auto& f : fragments)
        {
            if (!f.active)
                continue;

            f.life -= dt;
            if (f.life <= 0.0f)
            {
                f.active = false;
            }
        }
    }
}

void Grenade::Render(Gdiplus::Graphics& graphics, ImageManager& imgMgr)
{
    if (isActive) {

        Gdiplus::Bitmap* bmp = imgMgr.GetImage(spriteKey);
        if (!bmp) {
            DEBUG_MSG(L"[Grenade] GetImage 실패 -> 키가 잘못 되었거나 이미지가 로드되지 않음");
        }
        else {
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

            const float renderH = 15.0f;

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

    // 파편 렌더링
    if (!fragments.empty())
    {
        Gdiplus::Bitmap* fragBmp = imgMgr.GetImage(fragmentSpriteKey);
        if (!fragBmp)
        {
            DEBUG_MSG(L"[Grenade] 파편 이미지 로드 실패");
            return;
        }

        const float fragW = 10.0f; // 원본: 10
        const float fragH = 3.0f;  // 원본: 3

        Gdiplus::Matrix originMat;
        graphics.GetTransform(&originMat);

        for (const auto& f : fragments)
        {
            if (!f.active)
                continue;

            // 방향에 맞춰 회전
            float angleRad = std::atan2f(f.dir.Y, f.dir.X);
            float angleDeg = angleRad * 180.0f / PI;

            graphics.SetTransform(&originMat);
            graphics.TranslateTransform(f.pos.X, f.pos.Y);
            graphics.RotateTransform(angleDeg);
            graphics.TranslateTransform(-fragW * 0.5f, -fragH * 0.5f);

            Gdiplus::RectF dst(0.0f, 0.0f, fragW, fragH);

            graphics.DrawImage(
                fragBmp,
                dst,
                0.0f, 0.0f,
                10.0f, 3.0f,
                Gdiplus::UnitPixel
            );
        }

        graphics.SetTransform(&originMat);
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
    if (fuseRemain <= 0.0f) {
        fuseRemain = 0.0f;
        Explode();
    }
}

void Grenade::Explode()
{
    // 수류탄 본체는 더 이상 렌더하지 않음
    isActive = false;
    isInFuse = false;
    speed = 0.0f;

    fragments.clear();

    const int   FRAG_COUNT = 24;         // 360 / 15 -> 24갈래
    const float DEG2RAD = PI / 180.0f;
    const float FRAG_SPEED = 600.0f;    // 파편 속도
    const float FRAG_LIFETIME = 0.7f;      // 파편 존재 시간(초) - 필요에 따라 조절

    fragments.reserve(FRAG_COUNT);

    for (int i = 0; i < FRAG_COUNT; ++i)
    {
        float angleDeg = i * 15.0f;
        float rad = angleDeg * DEG2RAD;

        Fragment f{};
        f.pos = pos; // 수류탄 위치에서 시작
        f.dir = Gdiplus::PointF(std::cos(rad), std::sin(rad));
        f.speed = FRAG_SPEED;
        f.life = FRAG_LIFETIME;
        f.active = true;

        fragments.push_back(f);
    }
}

void Grenade::Collision(float deltaTime, Player* player)
{
    if (deltaTime <= 0.0f)
        return;

    if (fragments.empty())
        return;

    // 플레이어 충돌용 AABB (GameLoop에서 쓰는 90x50 기준)
    Gdiplus::RectF playerRect;
    bool hasPlayer = false;

    if (player)
    {
        Gdiplus::PointF pPos = player->GetPos();
        float halfW = 90.0f * 0.5f;
        float halfH = 50.0f * 0.5f;

        playerRect = Gdiplus::RectF(
            pPos.X - halfW,
            pPos.Y - halfH,
            90.0f,
            50.0f
        );
        hasPlayer = true;
    }

    for (auto& f : fragments)
    {
        if (!f.active)
            continue;
        if (f.life <= 0.0f)
            continue;

        float step = f.speed * deltaTime;
        if (step <= 0.0f)
            continue;

        Gdiplus::PointF prevPos = f.pos;

        // 1) 벽 충돌 검사
        bool hitWall = false;
        if (wall)
        {
            Gdiplus::PointF hitPoint;
            POINT hitCell{ -1, -1 };

            if (wall->Raycast(prevPos, f.dir, step, &hitPoint, &hitCell))
            {
                // 벽에 닿은 해당 파편만 제거
                f.active = false;
                hitWall = true;
            }
        }

        if (hitWall)
            continue; // 이미 사라진 파편은 더 처리 X

        // 2) 벽에 안 닿았으면 이동
        f.pos.X += f.dir.X * step;
        f.pos.Y += f.dir.Y * step;

        // 3) 플레이어 충돌 검사
        if (hasPlayer)
        {
            // 간단히 "점-사각형" 충돌로 처리
            if (playerRect.Contains(f.pos))
            {
                // 실제로는 여기에서 HP -= damage 1회 처리
                DEBUG_MSG(L"[Grenade] 파편이 플레이어에 명중했습니다! (1회 데미지 처리)");

                // 이 파편은 1회만 데미지 주고 사라짐
                f.active = false;
            }
        }
    }
}

bool Grenade::Bounce(const Gdiplus::PointF& prevPos, float step)
{
    if (!wall)
        return false;

    Gdiplus::PointF hitPoint;
    POINT hitCell{ -1, -1 };

    // Raycast: prevPos -> dir 방향으로 step 거리까지 검사
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
    case 0: n = Gdiplus::PointF(-1.0f, 0.0f); break; // 왼쪽 면 
    case 1: n = Gdiplus::PointF(1.0f, 0.0f); break; // 오른쪽 면 
    case 2: n = Gdiplus::PointF(0.0f, -1.0f); break; // 위쪽 면 
    case 3: n = Gdiplus::PointF(0.0f, 1.0f); break; // 아래쪽 면 
    }

    // 충돌 지점까지 실제로 이동한 거리
    float vx = hitPoint.X - prevPos.X;
    float vy = hitPoint.Y - prevPos.Y;
    float hitDist = std::sqrt(vx * vx + vy * vy);

    if (hitDist < 0.0f)
        hitDist = 0.0f;

    // 이번 프레임에서 남아 있는 거리 (반사 후에 쓸 것)
    float remain = step - hitDist;
    if (remain < 0.0f)
        remain = 0.0f;

    // 반사 벡터 
    float dot = dir.X * n.X + dir.Y * n.Y;
    Gdiplus::PointF reflect(
        dir.X - 2.0f * dot * n.X,
        dir.Y - 2.0f * dot * n.Y
    );

    // 반사 방향 정규화
    float len = std::sqrt(reflect.X * reflect.X + reflect.Y * reflect.Y);
    if (len > 0.0001f)
    {
        reflect.X /= len;
        reflect.Y /= len;
    }

    // 충돌 지점에 위치시키고, 벽 밖으로 아주 살짝만 밀어냄 (겹침 방지)
    const float epsilon = 0.5f;
    pos = hitPoint;
    pos.X += n.X * epsilon;
    pos.Y += n.Y * epsilon;

    // 속도 감쇠 
    const float bounceDamp = 0.6f;   
    speed *= bounceDamp;
    if (speed < 0.0f)
        speed = 0.0f;

    // 새 방향 저장
    dir = reflect;

    // 속도가 0이 아니고, 남은 거리가 있으면
    // 이번 프레임 남은 거리만큼 새 방향으로 추가 이동
    if (speed > 0.0f && remain > 0.0f)
    {
        // dir는 이미 정규화되어 있음
        pos.X += dir.X * remain;
        pos.Y += dir.Y * remain;
    }

    DEBUG_MSG(L"[Grenade] 벽 반사: hitDist=%.1f, remain=%.1f, speed=%.1f",
        hitDist, remain, speed);

    return true;
}

void Grenade::Reset()
{
    isActive = false;
    traveled = 0.0f;
    maxDistance = 0.0f;
    dir = Gdiplus::PointF(0.0f, 0.0f);
}
