#include "pch.h"
#include "SoundManager.h"

void SoundManager::LoadSound(const std::string& key, const std::string& filePath)
{
    // 이미 같은 key가 있다면 덮어쓰기
    for (auto& s : sounds)
    {
        if (s.key == key)
        {
            s.filepath = filePath;
            return;
        }
    }

    SoundEntry entry;
    entry.key = key;
    entry.filepath = filePath;
    sounds.push_back(entry);
}

const SoundManager::SoundEntry* SoundManager::FindSound(const std::string& key) const
{
    for (const auto& s : sounds)
    {
        if (s.key == key)
        {
            return &s;
        }
    }
    return nullptr;
}

void SoundManager::Play(const std::string& key)
{
    const SoundEntry* entry = FindSound(key);
    if (!entry)
    {
        OutputDebugStringA(("[SoundManager] Cannot find sound: " + key + "\n").c_str());
        return;
    }

    PlaySoundA(entry->filepath.c_str(), nullptr, SND_FILENAME | SND_ASYNC);
}

void SoundManager::PlayBGM(const std::string& key)
{
    const SoundEntry* entry = FindSound(key);
    if (!entry)
    {
        OutputDebugStringA(("[SoundManager] Cannot find BGM: " + key + "\n").c_str());
        return;
    }

    currentBGM = key;
    PlaySoundA(entry->filepath.c_str(), nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP);
}

void SoundManager::StopBGM()
{
    // 현재 구조에서는 그냥 PlaySound(nullptr)로 정지
    PlaySoundA(nullptr, nullptr, 0);
    currentBGM.clear();
}

void SoundManager::StopAll()
{
    PlaySoundA(nullptr, nullptr, 0);
}
