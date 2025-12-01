#pragma once

class Grenade {
public:
	bool	isActive;
	float	posX;
	float	posY;

public:
	int grenadeId;

	Grenade(int id, float x, float y) :
		isActive(true), posX(x), posY(y)
	{
		grenadeId = id;
	}
};