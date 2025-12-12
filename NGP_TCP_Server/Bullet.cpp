#include "Bullet.h"

constexpr float moveDistance = 3.0f;

Bullet::Bullet(int id) :
	posX(0.0f), posY(0.0f), dirAngle(0.0f)
{
	bulletId = id;
	isActive = false;
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
	if (isActive) {
		posX += moveDistance * std::cos(dirAngle);
		posY += moveDistance * std::sin(dirAngle);
	}
}

void Bullet::SetIsActive(bool is)
{
	isActive = is;
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