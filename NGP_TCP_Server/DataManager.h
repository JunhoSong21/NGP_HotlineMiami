#pragma once
#include <unordered_map>
#include <memory>
#include <mutex>
#include <cmath>
#include <array>
#include "Timer.h"
#include "Player.h"
#include "Bullet.h"
#include "Grenade.h"
#include "GlobalData.h"

constexpr int MAX_GRENADE_PER_PLAYER = 2;
class Player;
class Bullet;
class Grenade;

class DataManager {
private:
	std::unordered_map<int, std::unique_ptr<Player>>	playerData;
	std::unordered_map<int, std::unique_ptr<Bullet>>	bulletData;
	std::unordered_map<int, std::unique_ptr<Grenade>>	grenadeData;
	std::array<int, MAX_CLIENT_NUM> grenadeRemain{};

	DataManager();
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
	bool PlayerToBulletCollision(Player* player, Bullet* bullet);
	void PlayerToGrenadeCollision();

	bool TryConsumeGrenade(int playerId);
	void ResetGrenadeCount(int playerId);
};