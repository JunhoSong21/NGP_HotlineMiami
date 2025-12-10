#include "Grenade.h"

Grenade::Grenade(int id) :
	posX(0.0f), posY(0.0f)
{
	grenadeId = id;
	isActive = false;
	isExplode = false;
}

void Grenade::ChangeStateToExplode()
{
	isActive = false;
	isExplode = true;
}

void Grenade::ChangeStateToDebris()
{
	isExplode = false;
}

void Grenade::SetIsActive()
{
	isActive = true;
}

void Grenade::SetPos(float x, float y)
{
	posX = x;
	posY = y;
}

float Grenade::GetPosX() const
{
	return posX;
}

float Grenade::GetPosY() const
{
	return posY;
}

bool Grenade::GetIsActive() const
{
	return isActive;
}

bool Grenade::GetIsExplode() const
{
	return isExplode;
}