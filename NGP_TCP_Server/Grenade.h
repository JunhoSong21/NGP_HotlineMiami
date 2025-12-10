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
	Grenade(int id);

	void ChangeStateToExplode();
	void ChangeStateToDebris();

	void SetIsActive();
	void SetPos(float x, float y);

	float GetPosX() const;
	float GetPosY() const;

	bool GetIsActive() const;
	bool GetIsExplode() const;
};