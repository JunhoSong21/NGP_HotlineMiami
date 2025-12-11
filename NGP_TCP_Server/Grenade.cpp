#include "Grenade.h"
#include <cmath>

namespace {
	constexpr float GRENADE_SPEED = 280.0f;   // 초기 속도 (px/s)
	constexpr float GRENADE_FRICTION = 480.0f;   // 감속량 (px/s^2)
	constexpr float GRENADE_MIN_SPEED = 5.0f;     // 이 이하이면 정지로 판단
	constexpr float GRENADE_FUSE_TIME = 3.0f;     // 요구사항: 정지 후 3초
}

Grenade::Grenade(int id)
	: grenadeId(id)
	, posX(0.0f)
	, posY(0.0f)
	, velX(0.0f)
	, velY(0.0f)
	, isActive(false)
	, isExplode(false)
	, hasStopped(false)
	, fuseStarted(false)
	, fuseElapsed(0.0f)
{
}

void Grenade::Throw(float startX, float startY, float dirRadAngle)
{
    posX = startX;
    posY = startY;

    velX = std::cos(dirRadAngle) * GRENADE_SPEED;
    velY = std::sin(dirRadAngle) * GRENADE_SPEED;

    isActive = true;
    isExplode = false;
    hasStopped = false;
    fuseStarted = false;
    fuseElapsed = 0.0f;
}

void Grenade::Update(float deltaSeconds)
{
    if (!isActive || isExplode) {
        return;
    }

    // 1) 위치 업데이트
    posX += velX * deltaSeconds;
    posY += velY * deltaSeconds;

    // 2) 감속 적용
    float speed = std::sqrt(velX * velX + velY * velY);
    if (speed > 0.0f) {
        float decel = GRENADE_FRICTION * deltaSeconds;
        float newSpeed = speed - decel;
        if (newSpeed <= 0.0f) {
            velX = 0.0f;
            velY = 0.0f;
            speed = 0.0f;
        }
        else {
            float scale = newSpeed / speed;
            velX *= scale;
            velY *= scale;
            speed = newSpeed;
        }
    }

    // 3) 정지 판정 & 퓨즈 시작
    if (!hasStopped && speed <= GRENADE_MIN_SPEED) {
        hasStopped = true;
        velX = 0.0f;
        velY = 0.0f;
        fuseStarted = true;   // 여기서부터 3초 카운트 시작
        fuseElapsed = 0.0f;
    }

    // 4) 퓨즈 진행
    if (fuseStarted && !isExplode) {
        fuseElapsed += deltaSeconds;
        if (fuseElapsed >= GRENADE_FUSE_TIME) {
            ChangeStateToExplode();
        }
    }
}

void Grenade::ChangeStateToExplode()
{
    if (isExplode) {
        return;
    }
	isActive = false;
	isExplode = true;
}

void Grenade::ChangeStateToDebris()
{
	isExplode = false;
}

void Grenade::SetIsActive(bool active)
{
    isActive = active;
}

void Grenade::SetPos(float x, float y)
{
	posX = x;
	posY = y;
}

float Grenade::GetRemainFuse() const
{
    if (!fuseStarted || isExplode) {
        return 0.0f;
    }

    float remain = GRENADE_FUSE_TIME - fuseElapsed;
    if (remain < 0.0f) {
        remain = 0.0f;
    }
    return remain;
}