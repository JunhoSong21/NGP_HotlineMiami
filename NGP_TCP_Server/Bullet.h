#pragma once
#include <cmath>

class Bullet {
public:
	int bulletId;

private:
	bool	isActive;

	float	posX;
	float	posY;
	float	dirAngle;

public:
	Bullet(int id, float x, float y, float angle);
	bool IsActive() const;

	void Collision();

	void CalcPosbyAngle();

	void SetPos(float x, float y);
	void SetAngle(float angle);

	float GetPosX() const;
	float GetPosY() const;
	float GetAngle() const;

	bool GetIsActive() const;
};