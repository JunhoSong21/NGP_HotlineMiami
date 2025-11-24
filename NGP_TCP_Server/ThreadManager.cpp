#include "ThreadManager.h"

void ThreadManager::AddThread(SOCKET clientSock)
{
	std::lock_guard<std::mutex> lock(threadMutex);

	int clientThreadId = threadId;
	networkThreads[clientThreadId] = std::make_unique<NetworkThread>(clientThreadId, clientSock);

	//auto newPlayer = std::make_unique<Player>(threadId, threadId);
	//DataManager::GetInstance().AddPlayer(std::move(newPlayer));

	// 클라이언트가 가진 플레이어, 총알, 수류탄 정보를 쉽게 관리하기 위해
	// 쓰레드 id를 공용키로 사용할 수 있도록 한다.
	++threadId;
}

void ThreadManager::RemoveThread(int id)
{
	std::lock_guard<std::mutex> lock(threadMutex);

	auto it = networkThreads.find(id);
	if (it != networkThreads.end())
		networkThreads.erase(it);
}

int ThreadManager::ThreadCount()
{
	std::lock_guard<std::mutex> lock(threadMutex);
	return networkThreads.size();
}