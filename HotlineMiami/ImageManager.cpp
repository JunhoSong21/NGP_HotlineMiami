#include "pch.h"
#include "ImageManager.h"

ImageManager::ImageManager()
{

}

ImageManager::~ImageManager()
{
	Shutdown();
}

bool ImageManager::LoadImage(const std::wstring& filePath, const std::wstring& imageKey)
{
	if (0 != Images.count(imageKey))
		return true;

	Gdiplus::Bitmap* newBitmap = Gdiplus::Bitmap::FromFile(filePath.c_str());

	if (newBitmap == nullptr || newBitmap->GetLastStatus() != Gdiplus::Ok) {
		if (newBitmap)
			delete newBitmap;

		return false;
	}

	Images[imageKey] = newBitmap;
	return true;
}

Gdiplus::Bitmap* ImageManager::GetImage(const std::wstring& imageKey)
{
	auto it = Images.find(imageKey);

	if (it != Images.end()) {
		return it->second;
	}

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