#include "ThreadManager.h"

void ThreadManager::AddThread(SOCKET clientSock)
{
	std::lock_guard<std::mutex> lock(threadMutex);

	int clientThreadId = threadId++;
	networkThreads[clientThreadId] = std::make_unique<NetworkThread>(clientThreadId, clientSock);
}

void ThreadManager::RemoveThread(int id)
{
	std::lock_guard<std::mutex> lock(threadMutex);

	auto it = networkThreads.find(id);
	if (it != networkThreads.end())
		networkThreads.erase(it);
}