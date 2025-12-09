#include "Bullet.h"

constexpr float moveDistance = 1.0f;

Bullet::Bullet(int id, float x, float y, float angle) :
	posX(x), posY(y), dirAngle(angle)
{
	bulletId = id;
	isActive = true;
}

bool Bullet::IsActive() const
{
	return isActive;
}

void Bullet::Collision()
{
	if (isActive)
		isActive = false;
}

void Bullet::CalcPosbyAngle()
{
	float newPosX, newPosY;

	newPosX = posX + moveDistance * std::cos(dirAngle);
	newPosY = posY + moveDistance * std::sin(dirAngle);
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