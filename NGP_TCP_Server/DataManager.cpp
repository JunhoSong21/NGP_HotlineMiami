#include "DataManager.h"

using std::unique_ptr;
using std::lock_guard;
using std::mutex;

void DataManager::AddPlayer(unique_ptr<Player> player)
{
	lock_guard<mutex> lock(playerMapMutex);
	if (player) {
		playerData[player->playerId] = std::move(player);
		printf("player 추가 완료\n");
	}
	else
		printf("플레이어 데이터 nullptr\n");
}

void DataManager::AddBullet(unique_ptr<Bullet> bullet)
{

}

void DataManager::AddGrenade(unique_ptr<Grenade> grenade)
{

}

Player* DataManager::GetPlayer(int id)
{
	lock_guard<mutex> lock(playerMapMutex);
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
	return 0;
}

Grenade* DataManager::GetGrenade(int id)
{
	return 0;
}