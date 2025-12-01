#include "pch.h"
#include "SoundManager.h"

#include <xaudio2.h>
#include <fstream>
#include <cstring>
#include <objbase.h> 

#pragma comment(lib, "xaudio2.lib")

static bool LoadWaveFile(const std::string& filePath, WAVEFORMATEX& outFormat, std::vector<BYTE>& outData)
{
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        OutputDebugStringA(("[SoundManager] Cannot open wav file: " + filePath + "\n").c_str());
        return false;
    }

    auto read_u32 = [&](UINT32& v) {
        file.read(reinterpret_cast<char*>(&v), sizeof(UINT32));
        };

    auto read_u16 = [&](UINT16& v) {
        file.read(reinterpret_cast<char*>(&v), sizeof(UINT16));
        };

    // RIFF 헤더
    char riff[4];
    file.read(riff, 4);
    if (file.gcount() != 4 || std::strncmp(riff, "RIFF", 4) != 0) {
        OutputDebugStringA(("[SoundManager] Not a RIFF file: " + filePath + "\n").c_str());
        return false;
    }

    UINT32 riffSize = 0;
    read_u32(riffSize);

    char wave[4];
    file.read(wave, 4);
    if (file.gcount() != 4 || std::strncmp(wave, "WAVE", 4) != 0) {
        OutputDebugStringA(("[SoundManager] Not a WAVE file: " + filePath + "\n").c_str());
        return false;
    }

    bool fmtFound = false;
    bool dataFound = false;
    WAVEFORMATEX fmt{};
    std::vector<BYTE> data;

    while (file && (!fmtFound || !dataFound)) {
        char chunkId[4];
        UINT32 chunkSize = 0;

        file.read(chunkId, 4);
        if (file.gcount() != 4) {
            break;
        }
        read_u32(chunkSize);
        if (!file) {
            break;
        }

        if (std::strncmp(chunkId, "fmt ", 4) == 0) {
            // 기본 PCM 포맷(16바이트)만 처리
            UINT16 audioFormat = 0;
            UINT16 numChannels = 0;
            UINT32 sampleRate = 0;
            UINT32 byteRate = 0;
            UINT16 blockAlign = 0;
            UINT16 bitsPerSample = 0;

            read_u16(audioFormat);
            read_u16(numChannels);
            read_u32(sampleRate);
            read_u32(byteRate);
            read_u16(blockAlign);
            read_u16(bitsPerSample);

            // 확장 영역은 건너뛴다.
            if (chunkSize > 16) {
                file.seekg(chunkSize - 16, std::ios::cur);
            }

            fmt.wFormatTag = audioFormat;
            fmt.nChannels = numChannels;
            fmt.nSamplesPerSec = sampleRate;
            fmt.nAvgBytesPerSec = byteRate;
            fmt.nBlockAlign = blockAlign;
            fmt.wBitsPerSample = bitsPerSample;
            fmt.cbSize = 0;

            fmtFound = true;
        }
        else if (std::strncmp(chunkId, "data", 4) == 0) {
            data.resize(chunkSize);
            if (chunkSize > 0) {
                file.read(reinterpret_cast<char*>(data.data()), chunkSize);
            }
            dataFound = true;
        }
        else {
            file.seekg(chunkSize, std::ios::cur);
        }
    }

    if (!fmtFound || !dataFound) {
        OutputDebugStringA(("[SoundManager] Invalid wav file (missing fmt or data): " + filePath + "\n").c_str());
        return false;
    }

    outFormat = fmt;
    outData.swap(data);
    return true;
}

SoundManager::SoundManager()
    : xaudio(nullptr)
    , masterVoice(nullptr)
    , bgmVoice(nullptr)
    , comInitialized(false)
{
    // COM 초기화
    HRESULT hrCo = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (SUCCEEDED(hrCo)) {
        comInitialized = true;
    }
    else if (hrCo == RPC_E_CHANGED_MODE) {
        comInitialized = false;
        OutputDebugStringA("[SoundManager] COM already initialized in different mode (ok)\n");
    }
    else {
        OutputDebugStringA("[SoundManager] CoInitializeEx failed\n");
    }

    // XAudio2 엔진 생성
    HRESULT hr = XAudio2Create(&xaudio, 0, XAUDIO2_DEFAULT_PROCESSOR);
    if (FAILED(hr)) {
        char buf[128];
        sprintf_s(buf, "[SoundManager] XAudio2Create failed. hr=0x%08X\n", hr);
        OutputDebugStringA(buf);
        xaudio = nullptr;
        return;
    }

    // MasteringVoice 생성 (실제 출력 장치)
    hr = xaudio->CreateMasteringVoice(&masterVoice);
    if (FAILED(hr)) {
        char buf[128];
        sprintf_s(buf, "[SoundManager] CreateMasteringVoice failed. hr=0x%08X\n", hr);
        OutputDebugStringA(buf);

        xaudio->Release();
        xaudio = nullptr;
        masterVoice = nullptr;
        return;
    }

    OutputDebugStringA("[SoundManager] XAudio2 initialized successfully\n");
}

SoundManager::~SoundManager()
{
    StopBGM();

    for (auto* v : activeVoices) {
        if (v) {
            v->DestroyVoice();
        }
    }
    activeVoices.clear();

    if (masterVoice) {
        masterVoice->DestroyVoice();
        masterVoice = nullptr;
    }

    if (xaudio) {
        xaudio->Release();
        xaudio = nullptr;
    }

    if (comInitialized) {
        CoUninitialize();
        comInitialized = false;
    }
}

SoundManager::SoundEntry* SoundManager::FindSoundInternal(const std::string& key)
{
    for (auto& s : sounds) {
        if (s.key == key) {
            return &s;
        }
    }
    return nullptr;
}

const SoundManager::SoundEntry* SoundManager::FindSound(const std::string& key) const
{
    for (const auto& s : sounds) {
        if (s.key == key) {
            return &s;
        }
    }
    return nullptr;
}

void SoundManager::LoadSound(const std::string& key, const std::string& filePath)
{
    if (!xaudio) {
        OutputDebugStringA("[SoundManager] XAudio2 not initialized\n");
        return;
    }

    SoundEntry* entry = FindSoundInternal(key);
    if (!entry) {
        SoundEntry newEntry;
        newEntry.key = key;
        newEntry.filepath = filePath;
        sounds.push_back(newEntry);
        entry = &sounds.back();
    }
    else {
        entry->filepath = filePath;
    }

    // WAV 파일 로드
    if (!LoadWaveFile(filePath, entry->format, entry->audioData)) {
        entry->loaded = false;
        return;
    }

    entry->loaded = true;
    OutputDebugStringA(("[SoundManager] Loaded sound: " + key + " (" + filePath + ")\n").c_str());
}

void SoundManager::Play(const std::string& key)
{
    if (!xaudio) {
        return;
    }

    const SoundEntry* entry = FindSound(key);
    if (!entry || !entry->loaded) {
        OutputDebugStringA(("[SoundManager] Cannot play sound: " + key + "\n").c_str());
        return;
    }

    IXAudio2SourceVoice* voice = nullptr;
    HRESULT hr = xaudio->CreateSourceVoice(&voice, &entry->format);
    if (FAILED(hr)) {
        OutputDebugStringA(("[SoundManager] CreateSourceVoice failed for: " + key + "\n").c_str());
        return;
    }

    XAUDIO2_BUFFER buffer{};
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.AudioBytes = static_cast<UINT32>(entry->audioData.size());
    buffer.pAudioData = entry->audioData.data();

    hr = voice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) {
        OutputDebugStringA(("[SoundManager] SubmitSourceBuffer failed for: " + key + "\n").c_str());
        voice->DestroyVoice();
        return;
    }

    hr = voice->Start(0);
    if (FAILED(hr)) {
        OutputDebugStringA(("[SoundManager] Start failed for: " + key + "\n").c_str());
        voice->DestroyVoice();
        return;
    }

    activeVoices.push_back(voice);
}

void SoundManager::PlayBGM(const std::string& key)
{
    if (!xaudio) {
        return;
    }

    const SoundEntry* entry = FindSound(key);
    if (!entry || !entry->loaded) {
        OutputDebugStringA(("[SoundManager] Cannot find BGM: " + key + "\n").c_str());
        return;
    }

    // 기존 BGM 보이스 정리
    StopBGM();

    HRESULT hr = xaudio->CreateSourceVoice(&bgmVoice, &entry->format);
    if (FAILED(hr)) {
        OutputDebugStringA(("[SoundManager] CreateSourceVoice failed for BGM: " + key + "\n").c_str());
        bgmVoice = nullptr;
        return;
    }

    XAUDIO2_BUFFER buffer{};
    buffer.Flags = XAUDIO2_END_OF_STREAM;
    buffer.AudioBytes = static_cast<UINT32>(entry->audioData.size());
    buffer.pAudioData = entry->audioData.data();
    buffer.LoopCount = XAUDIO2_LOOP_INFINITE; // 전체 버퍼 무한 반복

    hr = bgmVoice->SubmitSourceBuffer(&buffer);
    if (FAILED(hr)) {
        OutputDebugStringA(("[SoundManager] SubmitSourceBuffer failed for BGM: " + key + "\n").c_str());
        bgmVoice->DestroyVoice();
        bgmVoice = nullptr;
        return;
    }

    hr = bgmVoice->Start(0);
    if (FAILED(hr)) {
        OutputDebugStringA(("[SoundManager] Start failed for BGM: " + key + "\n").c_str());
        bgmVoice->DestroyVoice();
        bgmVoice = nullptr;
        return;
    }

    currentBGM = key;
}

void SoundManager::StopBGM()
{
    if (bgmVoice) {
        bgmVoice->Stop(0);
        bgmVoice->DestroyVoice();
        bgmVoice = nullptr;
    }
    currentBGM.clear();
}

void SoundManager::StopAll()
{
    // 효과음 보이스 정리
    for (auto* v : activeVoices) {
        if (v) {
            v->Stop(0);
            v->DestroyVoice();
        }
    }
    activeVoices.clear();

    // BGM 정지
    StopBGM();
}

void SoundManager::Update()
{
    if (!xaudio) {
        return;
    }

    // 재생이 끝난 보이스를 정리
    for (size_t i = 0; i < activeVoices.size();) {
        IXAudio2SourceVoice* v = activeVoices[i];
        XAUDIO2_VOICE_STATE state{};
        v->GetState(&state);
        if (state.BuffersQueued == 0) {
            v->DestroyVoice();
            activeVoices.erase(activeVoices.begin() + i);
        }
        else {
            ++i;
        }
    }
}
