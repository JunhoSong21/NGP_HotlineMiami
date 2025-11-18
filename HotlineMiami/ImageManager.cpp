#include "pch.h"
#include "ImageManager.h"

ImageManager::ImageManager()
{

}

ImageManager::~ImageManager()
{
	Shutdown();
}

bool ImageManager::LoadSpriteImage(const std::wstring& filePath, const std::wstring& imageKey)
{
	if (0 != Images.count(imageKey))
		return true;

	Gdiplus::Bitmap* newBitmap = Gdiplus::Bitmap::FromFile(filePath.c_str());

	if (!newBitmap)
		DEBUG_MSG(L"이미지 로드 실패");

	Images[imageKey] = newBitmap;
	return true;
}

Gdiplus::Bitmap* ImageManager::GetImage(const std::wstring& imageKey)
{
	auto it = Images.find(imageKey);
	if (it != Images.end()) {
		Gdiplus::Bitmap* bitmap = it->second;
		if (bitmap && bitmap->GetLastStatus() == Gdiplus::Ok) {
			return bitmap;
		}
		else {
			DEBUG_MSG(L"이미지가 손상됨");
			return nullptr;
		}
	}

	DEBUG_MSG(L"이미지를 찾을 수 없음");
	return nullptr;
}

void ImageManager::Shutdown()
{
	for (auto const& pair : Images) {
		std::wstring key		= pair.first;
		Gdiplus::Bitmap* value	= pair.second;

		if (value)
			delete value;
	}

	Images.clear();
}