#pragma once

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

	void Collision();

	void SetPos(float x, float y);
	void SetAngle(float angle);

	float GetPosX() const;
	float GetPosY() const;
	float GetAngle() const;
};