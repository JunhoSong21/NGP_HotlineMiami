#pragma once
#include <atomic>

#include "Packet.h"

class Player {
public:
	int playerId;

private:
	std::atomic<int> hp;

	bool isAlive;

	std::atomic<float> posX;
	std::atomic<float> posY;
	float angle;

public:
	Player(int id);

	void CollisionBullet();
	void CollisionGrenade();

	void CalcPosbyFlag(uint16_t flag, float x, float y);

	void SetPosX(float x);
	void SetPosY(float y);
	void SetAngle(float x);

	int GetHp() const;
	bool GetIsAlive() const;

	float GetPosX() const;
	float GetPosY() const;
	float GetAngle() const;
};