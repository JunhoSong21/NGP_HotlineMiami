#pragma once

class Grenade {
public:
	int grenadeId;

private:
	bool	isActive;	// 捧么 咯何
	bool	isExplode;	// 气惯 咯何
	float	posX;
	float	posY;

public:
	Grenade(int id, float x, float y);

	void ChangeStateToExplode();
	void ChangeStateToDebris();

	void SetPos(float x, float y);
};