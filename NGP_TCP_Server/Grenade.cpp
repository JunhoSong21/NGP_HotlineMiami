#include "Grenade.h"

Grenade::Grenade(int id, float x, float y) :
	posX(x), posY(y)
{
	grenadeId = id;
	isActive = true;
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

void Grenade::SetPos(float x, float y)
{
	posX = x;
	posY = y;
}