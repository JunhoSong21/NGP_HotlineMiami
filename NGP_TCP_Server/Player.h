#pragma once

class Player {
public:
	float posX;
	float posY;
	float angle;

public:
	int playerId;

	Player(int id) :
		posX(0.f), posY(0.f), angle(0.f)
	{
		playerId = id;
	}
};