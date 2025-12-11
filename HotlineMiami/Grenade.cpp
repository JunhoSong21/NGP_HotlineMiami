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
    , fuseRemain(0.0f)
{
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

void Grenade::ApplyServerState(bool active, bool explode, float x, float y, float remainFuse)
{
    // 위치 / 상태 동기화
    pos = Gdiplus::PointF(x, y);
    isActive = active;
    fuseRemain = remainFuse;

    // exploded 플래그가 false → true 로 변하는 순간에만 파편 생성
    if (explode && !exploded) {
        exploded = true;
        SpawnFragments();
    }
    else if (!explode && exploded) {
        // 서버에서 다시 초기화된 경우라면 클라 상태도 리셋
        exploded = false;
        fragments.clear();
    }
}

void Grenade::Update(float deltaTime)
{
    // 파편만 클라 로컬에서 수명 관리
    if (!exploded) {
        return;
    }

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

        f.pos.X += f.vel.X * deltaTime;
        f.pos.Y += f.vel.Y * deltaTime;

        anyActive = true;
    }

    if (!anyActive) {
        // 파편 다 사라졌으면 그레네이드 자체도 렌더링 종료
        isActive = false;
    }
}

void Grenade::Render(Gdiplus::Graphics& graphics, ImageManager& imgMgr)
{
    // 아무 상태도 아니면 스킵
    // (아직 안 던졌고, 폭발도 아닌 상태)
    if (!isActive && !exploded) {
        return;
    }

    // 1) 폭발 전: 수류탄 본체 + (서버에서 내려온) 남은 시간 텍스트
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

        //  남은 시간 표시: 서버에서 받은 값(fuseRemain) 사용
        //  - fuseRemain은 0.0f ~ 3.0f 범위로 클라가 ApplyServerState에서 저장해 둔 값이라고 가정
        if (fuseRemain > 0.0f) {
            float t = fuseRemain;
            if (t < 0.0f) t = 0.0f;

            WCHAR buf[16];
            swprintf_s(buf, L"%.1f", t);   // 소수점 한 자리

            Gdiplus::Font       font(L"Consolas", 10.0f);
            Gdiplus::SolidBrush brush(Gdiplus::Color(255, 255, 0, 0));

            Gdiplus::PointF textPos(pos.X + 10.0f, pos.Y - 20.0f);
            graphics.DrawString(buf, -1, &font, textPos, &brush);
        }
    }
    // 2) 폭발 후: 파편 렌더링
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
