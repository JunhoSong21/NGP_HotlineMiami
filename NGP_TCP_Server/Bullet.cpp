#include "Bullet.h"

Bullet::Bullet(int id, float x, float y, float angle) :
	posX(x), posY(y), dirAngle(angle)
{
	bulletId = id;
	isActive = true;
}

void Bullet::Collision()
{
	if (isActive)
		isActive = false;
}

void Bullet::SetPos(float x, float y)
{
	posX = x;
	posY = y;
}

void Bullet::SetAngle(float angle)
{
	dirAngle = angle;
}

float Bullet::GetPosX() const
{
	return posX;
}

float Bullet::GetPosY() const
{
	return posY;
}

float Bullet::GetAngle() const
{
	return dirAngle;
}