#include "Player.h"

constexpr int Bullet_Damage		= 15;
constexpr int Grenade_Damage	= 40;

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

void Player::SetPos(float x, float y)
{
	posX = x;
	posY = y;
}

float Player::GetPosX()
{
	return posX;
}

float Player::GetPosY()
{
	return posY;
}