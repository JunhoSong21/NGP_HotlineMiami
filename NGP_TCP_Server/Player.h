#pragma once
#include <atomic>

class Player {
public:
	int playerId;

private:
	std::atomic<int> hp;

	bool isAlive;

	float posX;
	float posY;
	float angle;

public:
	Player(int id);

	void CollisionBullet();
	void CollisionGrenade();

	void SetPos(float x, float y);
	void SetAngle(float x);

	float GetPosX() const;
	float GetPosY() const;
	float GetAngle() const;
};