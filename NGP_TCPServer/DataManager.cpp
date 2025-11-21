#include "DataManager.h"

void DataManager::AddPlayer(std::unique_ptr<Player> player)
{
	std::lock_guard<std::mutex> lock(playerMapMutex);
	if (player)
		playerData[player->playerId] = std::move(player);
	else
		printf("플레이어 데이터 nullptr");
}

void DataManager::AddBullet(std::unique_ptr<Bullet> bullet)
{

}

void DataManager::AddGrenade(std::unique_ptr<Grenade> grenade)
{

}

Player* DataManager::GetPlayer(int id)
{

}

Bullet* DataManager::GetBullet(int id)
{

}

Grenade* DataManager::GetGrenade(int id)
{

}