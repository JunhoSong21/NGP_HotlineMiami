// Grenade.cpp
#include "pch.h"
#include "Grenade.h"
#include "Player.h"

namespace {
    constexpr float GRENADE_FUSE_TIME = 3.0f;   // 3초 뒤 폭발 
    constexpr int   FRAG_COUNT = 24;     // 파편 개수
    constexpr float FRAG_SPEED = 300.0f; // px/s 
    constexpr float FRAG_LIFE_TIME = 0.4f;   // 파편 수명
}

Grenade::Grenade()
    : spriteKey(L"sprGrenade")
    , pos(0.0f, 0.0f)
    , isActive(false)
    , exploded(false)
    , fragmentSpriteKey(L"sprGrenadeFragment")
    , fuseTime(0.0f)
    , maxFuseTime(GRENADE_FUSE_TIME)
{
}

void Grenade::Init()
{
    // 수류탄 상태 초기화
    pos = Gdiplus::PointF(0.0f, 0.0f);
    isActive = false;
    exploded = false;
    fuseTime = 0.0f;
    fragments.clear();
}

void Grenade::LoadGrenadeImage(ImageManager& imgMgr)
{
    // 수류탄 이미지 로드
    imgMgr.LoadSpriteImage(L"Resource/Grenade/sampleGrenade.png", spriteKey);

    // 파편 이미지 로드
    imgMgr.LoadSpriteImage(L"Resource/Grenade/sprGrenadeFragment.png", fragmentSpriteKey);
}

void Grenade::SpawnFragments()
{
    fragments.clear();
    fragments.reserve(FRAG_COUNT);

    const float twoPi = 3.1415926535f * 2.0f;

    for (int i = 0; i < FRAG_COUNT; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(FRAG_COUNT);
        float angle = twoPi * t;

        Fragment f{};
        f.pos = pos; // 중심 = 현재 수류탄 위치
        f.vel = Gdiplus::PointF(std::cos(angle) * FRAG_SPEED,
            std::sin(angle) * FRAG_SPEED);
        f.life = FRAG_LIFE_TIME;
        f.active = true;

        fragments.push_back(f);
    }
}

void Grenade::Update(float deltaTime)
{
    if (!isActive) {
        return;
    }

    // 아직 폭발 전이면 타이머 줄이기
    if (!exploded) {
        fuseTime -= deltaTime;
        if (fuseTime <= 0.0f) {
            fuseTime = 0.0f;
            exploded = true;

            // 폭발 시점에 파편 생성 (VFX)
            SpawnFragments();
        }
    }

    // 폭발 후에는 파편만 업데이트
    if (exploded) {
        bool anyActive = false;

        for (auto& f : fragments) {
            if (!f.active) {
                continue;
            }

            f.life -= deltaTime;
            if (f.life <= 0.0f) {
                f.active = false;
                continue;
            }

            // 단순 선형 이동
            f.pos.X += f.vel.X * deltaTime;
            f.pos.Y += f.vel.Y * deltaTime;
            anyActive = true;
        }

        // 모든 파편 수명이 끝나면 수류탄 비활성화
        if (!anyActive) {
            isActive = false;
        }
    }
}

void Grenade::Render(Gdiplus::Graphics& graphics, ImageManager& imgMgr)
{
    if (!isActive) {
        return;
    }

    // 1) 폭발 전: 수류탄 본체 그리기 + 남은 시간 표시 (원하면)
    if (!exploded) {
        Gdiplus::Bitmap* bmp = imgMgr.GetImage(spriteKey);
        if (bmp) {
            const float w = static_cast<float>(bmp->GetWidth());
            const float h = static_cast<float>(bmp->GetHeight());

            Gdiplus::RectF dest(
                pos.X - w * 0.5f,
                pos.Y - h * 0.5f,
                w,
                h
            );
            graphics.DrawImage(bmp, dest);
        }

        // 남은 시간 텍스트(선택 사항)
        // 필요 없으면 이 블록 통째로 지워도 됨
        if (maxFuseTime > 0.0f) {
            float t = fuseTime;
            if (t < 0.0f) t = 0.0f;

            WCHAR buf[16];
            swprintf_s(buf, L"%.1f", t);

            Gdiplus::Font font(L"Consolas", 10.0f);
            Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));

            Gdiplus::PointF textPos(pos.X + 10.0f, pos.Y - 20.0f);
            graphics.DrawString(buf, -1, &font, textPos, &brush);
        }
    }
    // 2) 폭발 후: 파편만 그리기
    else {
        Gdiplus::Bitmap* fragBmp = imgMgr.GetImage(fragmentSpriteKey);
        if (!fragBmp) {
            return;
        }

        const float fw = static_cast<float>(fragBmp->GetWidth());
        const float fh = static_cast<float>(fragBmp->GetHeight());

        for (const auto& f : fragments) {
            if (!f.active) {
                continue;
            }

            Gdiplus::RectF dst(
                f.pos.X - fw * 0.5f,
                f.pos.Y - fh * 0.5f,
                fw,
                fh
            );
            graphics.DrawImage(fragBmp, dst);
        }
    }
}

// 던졌다 표시만
void Grenade::Throw(const Gdiplus::PointF& startPos, const Gdiplus::PointF& targetPos)
{
    // 지금은 단일 수류탄만 허용 (이미 날아가는 중이면 무시)
    if (isActive)
        return;

    pos = startPos;
    isActive = true;
    exploded = false;
    fuseTime = maxFuseTime;
    fragments.clear();

    (void)targetPos;
}

