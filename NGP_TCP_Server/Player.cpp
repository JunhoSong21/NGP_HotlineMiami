#include "Player.h"

constexpr float Bullet_Damage	= 15.0f;
constexpr float Grenade_Damage	= 40.0f;

Player::Player(int id) :
	hp(100),
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
	if ((flag & (KEY_MOVE_A | KEY_MOVE_D)) == (KEY_MOVE_A | KEY_MOVE_D)); // A, D
	else if ((flag & KEY_MOVE_A) == KEY_MOVE_A) { // A 단독
		SetPosX(x -= 3.0f);
	}
	else if ((flag & KEY_MOVE_D) == KEY_MOVE_D) { // D 단독
		SetPosX(x += 3.0f);
	}

	if ((flag & (KEY_MOVE_W | KEY_MOVE_S)) == (KEY_MOVE_W | KEY_MOVE_S)); // W, S
	else if ((flag & KEY_MOVE_W) == KEY_MOVE_W) { // W 단독
		SetPosY(y -= 3.0f);
	}
	else if ((flag & KEY_MOVE_S) == KEY_MOVE_S) { // S 단독
		SetPosY(y += 3.0f);
	}
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