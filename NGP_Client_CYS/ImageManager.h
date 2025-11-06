#pragma once
#include <string>
#include <map>
#include <gdiplus.h>

class ImageManager {
private:
	// 상태값을 key, 비트맵(이미지)를 value로 하는 map container
	std::map<std::wstring, Gdiplus::Bitmap*> Images;

public:
	ImageManager();
	~ImageManager();

	bool LoadSpriteImage(const std::wstring& filePath, const std::wstring& imageKey);

	Gdiplus::Bitmap* GetImage(const std::wstring& imageKey);

	void Shutdown();
};

