#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>

#include "Player.h"
#include "Bullet.h"
#include "Grenade.h"

class Player;
class Bullet;
class Grenade;

class DataManager {
private:
	std::unordered_map<int, std::unique_ptr<Player>>	playerData;
	std::mutex playerMapMutex;
	std::unordered_map<int, std::unique_ptr<Bullet>>	bulletData;
	std::mutex bulletMapMutex;
	std::unordered_map<int, std::unique_ptr<Grenade>>	grenadeData;
	std::mutex grenadeMapMutex;

	DataManager() = default;
	~DataManager() = default;
	DataManager(const DataManager&) = delete;
	DataManager& operator=(const DataManager&) = delete;

public:
	static DataManager& GetInstance()
	{
		static DataManager instance;
		return instance;
	}

	void AddPlayer(std::unique_ptr<Player> player);
	void AddBullet(std::unique_ptr<Bullet> bullet);
	void AddGrenade(std::unique_ptr<Grenade> grenade);

	Player* GetPlayer(int id);
	Bullet* GetBullet(int id);
	Grenade* GetGrenade(int id);

	void CollisionCheck();
	bool PlayerToBulletCollision();
	bool PlayerToGrenadeCollision();
};