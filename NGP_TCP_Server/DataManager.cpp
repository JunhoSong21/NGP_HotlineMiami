#include "DataManager.h"
#include "GlobalData.h"

using std::unique_ptr;
using std::lock_guard;
using std::mutex;
using std::hypot;

constexpr float PLAYER_HITBOX_DISTANCE = 1.0f;

// DataManager.cpp
DataManager::DataManager()
{
	grenadeRemain.fill(MAX_GRENADE_PER_PLAYER);
}

bool DataManager::TryConsumeGrenade(int playerId)
{
	if (playerId < 0 || playerId >= MAX_CLIENT_NUM) {
		return false;
	}
	if (grenadeRemain[playerId] <= 0) {
		return false;
	}
	--grenadeRemain[playerId];
	return true;
}

void DataManager::ResetGrenadeCount(int playerId)
{
	if (playerId < 0 || playerId >= MAX_CLIENT_NUM) {
		return;
	}
	grenadeRemain[playerId] = MAX_GRENADE_PER_PLAYER;
}

void DataManager::AddPlayer(unique_ptr<Player> player)
{
	if (player) {
		playerData[player->playerId] = std::move(player);
		printf("player 추가 완료\n");
	}
	else
		printf("player 데이터 nullptr\n");
}

void DataManager::AddBullet(unique_ptr<Bullet> bullet)
{
	if (bullet) {
		bulletData[bullet->bulletId] = std::move(bullet);
		printf("bullet 추가 완료\n");
	}
	else
		printf("bullet 데이터 nullptr\n");
}

void DataManager::AddGrenade(unique_ptr<Grenade> grenade)
{
	if (grenade) {
		grenadeData[grenade->grenadeId] = std::move(grenade);
		printf("Grenade 추가 완료\n");
	}
	else
		printf("Grenade 데이터 nullptr\n");
}

Player* DataManager::GetPlayer(int id)
{
	auto it = playerData.find(id);
	if (it != playerData.end())
		return it->second.get();
	else {
		printf("GetPlayer() nullptr\n");
		return nullptr;
	}
}

Bullet* DataManager::GetBullet(int id)
{
	auto it = bulletData.find(id);
	if (it != bulletData.end())
		return it->second.get();
	else {
		printf("GetBullet() nullptr\n");
		return nullptr;
	}
}

Grenade* DataManager::GetGrenade(int id)
{
	auto it = grenadeData.find(id);
	if (it != grenadeData.end())
		return it->second.get();
	else {
		printf("GetGrenade() nullptr\n");
		return nullptr;
	}
}

void DataManager::CollisionCheck()
{
	for (int i = 0; i < MAX_CLIENT_NUM; ++i) {
		for (int j = 0; j < MAX_CLIENT_NUM; ++j) {
			if (i == j)
				continue;

			Player* player = GetPlayer(i);
			Bullet* bullet = GetBullet(j);
			//Grenade* grenade = GetGrenade(j);

			if (!player)
				printf("nullptr Player\n");
			if (!bullet)
				printf("nullptr Bullet\n");

			if (PlayerToBulletCollision(player, bullet)) {
				player->CollisionBullet();
				bullet->Collision();
				Timer::GetInstance().SetBulletArray(j);
			}
			//PlayerToGrenadeCollision();
		}
	}
}

bool DataManager::PlayerToBulletCollision(Player* player, Bullet* bullet)
{
	float distance = hypot(player->GetPosX() - bullet->GetPosX(), player->GetPosY() - bullet->GetPosY());

	return PLAYER_HITBOX_DISTANCE > distance;
}

void DataManager::PlayerToGrenadeCollision()
{
	return;
}