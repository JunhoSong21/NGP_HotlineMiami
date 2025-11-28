#include "DataManager.h"

void DataManager::AddPlayer(std::unique_ptr<Player> player)
{
	std::lock_guard<std::mutex> lock(playerMapMutex);
	if (player) {
		playerData[player->playerId] = std::move(player);
		printf("player 추가 완료\n");
	}
	else
		printf("플레이어 데이터 nullptr\n");
}

void DataManager::AddBullet(std::unique_ptr<Bullet> bullet)
{

}

void DataManager::AddGrenade(std::unique_ptr<Grenade> grenade)
{

}

Player* DataManager::GetPlayer(int id)
{
	return 0;
}

Bullet* DataManager::GetBullet(int id)
{
	return 0;
}

Grenade* DataManager::GetGrenade(int id)
{
	return 0;
}