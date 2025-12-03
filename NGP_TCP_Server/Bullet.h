#pragma once

class Bullet {
public:
	bool	isActive;
	float	posX;
	float	posY;
	float	dirAngle;

public:
	int bulletId;

	Bullet(int id, float x, float y, float angle) :
		isActive(true), posX(x), posY(y), dirAngle(angle)
	{
		bulletId = id;
	}
};