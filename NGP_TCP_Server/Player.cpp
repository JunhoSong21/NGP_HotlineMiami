#include "Player.h"

constexpr float Bullet_Damage	= 15.0f;
constexpr float Grenade_Damage	= 40.0f;

Player::Player(int id) :
	hp(100.0f),
	isAlive(true),
	posX(0.0f),
	posY(0.0f),
	angle(0.0f)
{
	playerId = id;
}

void Player::CollisionBullet()
{
	if (isAlive)
		hp -= Bullet_Damage;

	if (hp <= 0) {
		isAlive = false;
		hp = 0;
	}
}

void Player::CollisionGrenade()
{
	if (isAlive)
		hp -= Grenade_Damage;

	if (hp <= 0) {
		isAlive = false;
		hp = 0;
	}
}

void Player::CalcPosbyFlag(uint16_t flag, float x, float y)
{
	float dx = 0.0f;
	float dy = 0.0f;
	const float speed = 4.0f;

	if ((flag & KEY_MOVE_A) == KEY_MOVE_A) dx -= 1.0f;
	if ((flag & KEY_MOVE_D) == KEY_MOVE_D) dx += 1.0f;
	if ((flag & KEY_MOVE_W) == KEY_MOVE_W) dy -= 1.0f;
	if ((flag & KEY_MOVE_S) == KEY_MOVE_S) dy += 1.0f;

	if (dx == 0.0f && dy == 0.0f) {
		SetPosX(x);
		SetPosY(y);
		return;
	}

	// Á¤±ÔÈ­
	float len = std::sqrt(dx * dx + dy * dy);
	dx /= len;
	dy /= len;

	x += dx * speed;
	y += dy * speed;

	SetPosX(x);
	SetPosY(y);
}

void Player::SetPosX(float x)
{
	posX.store(x);
}

void Player::SetPosY(float y)
{
	posY.store(y);
}

void Player::SetAngle(float x)
{
	angle = x;
}

float Player::GetHp() const
{
	return hp;
}

bool Player::GetIsAlive() const
{
	return isAlive;
}

float Player::GetPosX() const
{
	return posX;
}

float Player::GetPosY() const
{
	return posY;
}

float Player::GetAngle() const
{
	return angle;
}